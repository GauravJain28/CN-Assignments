import sys
import threading
import socket

PORT = 8828
username = sys.argv[1]
SERVER_IP = sys.argv[2]

#Function for parsing input message by client
def input_parse(mess):
    recp = ""
    message =""
    wrongform = False

    if mess[0] == '@':
        k = mess.find(':')
        if k == -1:
            wrongform = True
        recp = mess[1:k]
        message = mess[k+1:]
        message = message.strip()
        if recp == '' or message=='':
            wrongform = True
    else:
        wrongform = True

    return wrongform,recp,message

#Function for parsing forwarded message by server
def forward_parse(mess):

    l = mess.split('\n')
    l0 = l[0].split(' ')
    typ = l0[0]
    sender = l0[1]
    l1 = l[1].split(' ')
    cl = int(l1[1])
    mess = l[3]

    wrongform = False
    if typ != 'FORWARD' or sender == '' or mess == '' or len(mess) != cl:
        wrongform = True

    return wrongform,sender,mess
    
#------------------------------------------------------------------------------------------#
'''
Function for execution in send socket thread
'''
def send(sock):
    while True:
        mess = input()

        #parse input
        wrongform,recp,message = input_parse(mess)

        #if wrong format print help
        if wrongform:
            print('Use the following format for sending messages:')
            print('@[RECIPIENT NAME]: Message\n')
            continue
        
        if recp == username:
            print('Receiver and Username are same')
            continue

        #adding header to message
        messagetosend = 'SEND {}\nContent-length: {}\n\n{}'.format(recp,len(message),message)
        try:
            sock.send(bytes(messagetosend,'utf-8'))
        except:
            print('Server Inactive')
            break

        #waiting for server's response    
        try:
            server_mess = sock.recv(2048).decode()
        except:
            print('Server Inactive')
            break

        #parse message of server
        if server_mess == 'SEND {}\n\n'.format(recp):
            print('Message delivered to {}'.format(recp))
        elif server_mess == 'ERROR 102 Unable to send\n\n':
            print('ERROR 102 : Unable to Send : Message not delivered to {}'.format(recp))
        elif server_mess == 'ERROR 103 Header Incomplete\n\n':
            print('ERROR 103 : Incomplete Header : Message not delivered to {}'.format(recp))
            print('Connection closed by server')
            break
        else:
            print('Unknown Error')
            break

    sock.close()

#------------------------------------------------------------------------------------------#
'''
Function for execution in recv socket thread
'''       
def recv(sock):
    while True:
        try:
            mess = sock.recv(2048).decode()  
        except:
            print('Server Inactive')
            break

        #parse the message forwarded by server
        wform,sender,message = forward_parse(mess)

        #send response to server depending on forwarded message
        if wform:
            response = 'ERROR 103 Header Incomplete\n\n'
                
        else:
            response = 'RECEIVED {}\n\n'.format(sender)
            print('{}: {}'.format(sender,message))
                
        #response sent to server
        try:
            sock.send(bytes(response,'utf-8'))
        except:
            print('Server Inactive')
            break
  
    sock.close()

#------------------------------------------------------------------------------------------#

if username == 'ALL':
    print('ALL is reserved keyword, use any other username')
    sys.exit()

#------------------------------------------------------------------------------------------#
'''
Send socket initialized and request for registration is sent
'''
send_client = socket.socket()
send_client.connect((SERVER_IP,PORT))

send_reg_message = 'REGISTER TOSEND {}\n\n'.format(username)
try:
    send_client.send(bytes(send_reg_message,'utf-8'))
except:
    print('Server Inactive')
    sys.exit()

#if acknowledged by server continue else if incorrect username throw (ERROR100)
try:
    send_ack_message = send_client.recv(2048).decode()
except:
    print('No server response')
    sys.exit()

#username is okay and send socket is registered
if send_ack_message == 'REGISTERED TOSEND {}\n\n'.format(username):
    #thread for send socket started
    print('{} : REGISTERED TOSEND'.format(username))
    send_th = threading.Thread(target=send,args=(send_client,))
    #send_th.start()

#username is malformed
elif send_ack_message == 'ERROR 100 Malformed username\n\n':   
    print('ERROR 100: Malformed Username')
    print('Restart the application...')
    print('Exiting...')
    sys.exit()

#reply to any other query
else:
    print('ERROR 101 : No user registered')
    print('Exiting...')
    sys.exit()

#------------------------------------------------------------------------------------------#
'''
Recv socket initialized and request for registration is sent
'''

recv_client = socket.socket()
recv_client.connect((sys.argv[2],PORT))

recv_reg_message = 'REGISTER TORECV {}\n\n'.format(username)

try:
    recv_client.send(bytes(recv_reg_message,'utf-8'))
except:
    print('Server is unavailable')
    sys.exit()

#if acknowledged by server continue elseif incorrect username (ERROR100)
try:
    recv_ack_message = recv_client.recv(2048).decode()
except:
    print('No response from server')

#username is okay and recv socket is registered
if recv_ack_message =='REGISTERED TORECV {}\n\n'.format(username):
    #thread for recv
    print('{} : REGISTERED TORECV'.format(username))
    recv_th = threading.Thread(target=recv,args=(recv_client,))
    #recv_th.start()

#username is malformed
elif recv_ack_message == 'ERROR 100 Malformed username\n\n':   
    print('ERROR 100 : Malformed Username')
    print('Restart the application...')
    print('Exiting...')
    sys.exit()

#reply to any other query
else:
    print('ERROR 101')
    print('Exiting...')
    sys.exit()
#------------------------------------------------------------------------------------------#

send_th.start()
recv_th.start()
