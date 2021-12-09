import socket
import threading

PORT = 8828
map = {}

#Function for parsing message sent by client
def message_parse(mess):

    l = mess.split('\n')
    l0 = l[0].split(' ')
    typ = l0[0]
    recvr = l0[1]
    l1 = l[1].split(' ')
    cl = int(l1[1])
    mess = l[3]

    wrongform = False
    if typ != 'SEND' or recvr == '' or mess == '' or len(mess) != cl:
        wrongform = True

    return wrongform,recvr,mess

#------------------------------------------------------------------------------#
'''
overall process for all threads
'''
def process(sock):
    #.........................................................................#
    #registering a socket of a client
    try:
        head = sock.recv(2048).decode()
    except:
        print('Client Inactive')
        sock.close()
        return 

    #.........................................................................#
    l = head.split('\n')
    l = l[0].split(' ')
    cont = False                    #boolean for continuing the process

    if l[0]=='REGISTER' and l[1]=='TOSEND' and l[2].isalnum():
        username = l[2]
        cont = True
        print('{} : REGISTERED TOSEND SOCKET'.format(username))
        cli_response = 'REGISTERED TOSEND {}\n\n'.format(username)  
        
    elif l[0]=='REGISTER' and l[1]=='TORECV' and l[2].isalnum():
        username = l[2]
        map[username] = sock
        print('{} : REGISTERED TORECV SOCKET'.format(username)) 
        cli_response = 'REGISTERED TORECV {}\n\n'.format(username)
        
    elif len(l)>2 and not l[2].isalnum():
        print('ERROR 100 : Malformed username : {}'.format(l[2]))
        cli_response = 'ERROR 100 Malformed username\n\n'
        
    else:
        print('ERROR 101 : No user registered')
        cli_response = 'ERROR 101 No user registered\n\n'
        
        
    #response to client for registration
    sock.send(bytes(cli_response,'utf-8'))

    #.........................................................................#
    if not cont:
        return

    #.........................................................................#
    #server will recieve messages and forward them
    while True:
        try:
            mess = sock.recv(2048).decode()
        except:
            print('Client Inactive')
            break

        wform,recvr,message = message_parse(mess)
        #...........................................................#
        if wform == True:
            print('ERROR 103 : Incomplete Header : Message not delivered from {} to {}'.format(username,recvr))
            response = 'ERROR 103 Header Incomplete\n\n'
            sock.send(bytes(response,'utf-8'))
            sock.close()
            del map[username]
            break
        #...........................................................#
        else:
            if recvr in map.keys():
                
                forw_sock = map[recvr]
                forw_mess = 'FORWARD {}\nContent-length: {}\n\n{}'.format(username,len(message),message)
                #...........................................................#
                #forward to receiver
                try:
                    forw_sock.send(bytes(forw_mess,'utf-8'))
                except:
                    print('ERROR 102 : Unable to Send : Message not delivered from {} to {}'.format(username,recvr))
                    response = 'ERROR 102 Unable to send\n\n'
                    sock.send(bytes(response,'utf-8'))

                #...........................................................#
                #response from receiver
                try:
                    recv_mess = forw_sock.recv(2048).decode()
                except:
                    print('ERROR 102 : Unable to Send : Message not delivered from {} to {}'.format(username,recvr))
                    response = 'ERROR 102 Unable to send\n\n'
                    sock.send(bytes(response,'utf-8'))


                if recv_mess == 'RECEIVED {}\n\n'.format(username):
                    print('Message forwarded from {} to {}'.format(username,recvr))
                    sock.send(bytes('SEND '+recvr+'\n\n','utf-8'))
                    

                elif recv_mess == 'ERROR 103 Header Incomplete\n\n':
                    print('ERROR 103 : Incomplete Header : Message not delivered from {} to {}'.format(username,recvr))
                    print('Closing connection of {}'.format(username))
                    response = 'ERROR 103 Header Incomplete\n\n'
                    sock.send(bytes(response,'utf-8'))
                    sock.close()
                    del map[username]
                    break 

                else:
                    print('Unknown Error')
                    break
            #...........................................................#
            elif recvr == 'ALL' and len(map)>1:

                for entry in map:

                    if entry != username:
                        forw_sock = map[entry]
                        forw_mess = 'FORWARD {}\nContent-length: {}\n\n{}'.format(username,len(message),message)
                        
                        #forward to receiver
                        try:
                            forw_sock.send(bytes(forw_mess,'utf-8'))
                        except:
                            print('ERROR 102 : Unable to Send : Message not delivered from {} to {}'.format(username,entry))
                            response = 'ERROR 102 Unable to send\n\n'
                            sock.send(bytes(response,'utf-8'))
                            break

                        #response from receiver
                        try:
                            recv_mess = forw_sock.recv(2048).decode()
                        except:
                            print('ERROR 102 : Unable to Send : Message not delivered from {} to {}'.format(username,entry))
                            response = 'ERROR 102 Unable to send\n\n'
                            sock.send(bytes(response,'utf-8'))
                            break
                            #sock.send(bytes(response,'utf-8'))


                        if recv_mess == 'RECEIVED {}\n\n'.format(username):
                            #print('Message forwarded from {} to {}'.format(username,recvr))
                            response = 'SEND '+recvr+'\n\n'
                            

                        elif recv_mess == 'ERROR 103 Header Incomplete\n\n':
                            print('ERROR 103 : Incomplete Header : Message not delivered from {} to {}'.format(username,entry))
                            print('Closing connection of {}'.format(username))
                            response = 'ERROR 103 Header Incomplete\n\n'
                            sock.send(bytes(response,'utf-8'))
                            #close sender socket
                            sock.close()
                            del map[username]
                            break
                        
                        else:
                            print('Unknown Error')
                            break

                
                if response == 'SEND {}\n\n'.format(recvr):
                    print('Message forwarded from {} to {}'.format(username,'ALL'))
                    sock.send(bytes(response,'utf-8'))

            #...........................................................#
            else:
                print('ERROR 102 : Unable to Send : Message not delivered from {} to {} ; {} does not exist'.format(username,recvr,recvr))
                response = 'ERROR 102 Unable to send\n\n'
                sock.send(bytes(response,'utf-8'))

            

#------------------------------------------------------------------------------#
N = 8                                              
server = socket.socket()
server.bind(('localhost',PORT))

server.listen(N)
print('Waiting for connections...')

while True:
    cli,addr = server.accept()
    thd = threading.Thread(target=process,args=(cli,))
    thd.start()

#------------------------------------------------------------------------------#
