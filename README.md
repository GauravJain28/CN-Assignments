# Computer Networks Assignments
These assignments are of the course COL334 : Computer Networks offered in First (Diwali) Semester, 2021-22 at IIT Delhi. There are 3 assignments in this repo and all of them are implemented in either ```C++``` or ```Python```.

## Assignment 1
The problem statement of the assignment can be accessed from [here](./A1/COL334_A1_PS.pdf). 

The assignment consists of three parts-
* Introduction to Networking tools like ```ifconfig```, ```nslookup```, ```ping``` and ```traceroute```. 
* Introduction to packet sniffing using ```Wireshark```. 
* Implementation of ```traceroute``` using ```ping```.

### Solution
* The results of this assignment are present in the report which can be accessed from [here](./A1/COL334_A1_Report.pdf).
* The third part is implemented in ```C++``` and the plots are generated using a ```Python``` script. These files are present [here](./A1/)
 

## Assignment 2
The problem statement of the assignment can be accessed from [here](./A2/COL334_A2_PS.pdf).

In this assignment, we build a chat application that allows users to send
plain text messages with one another. Users can direct messages to other users using an @ prefix, and the server needs to forward these messages to the intended recipients. The message could be intended to be sent for a single client (Unicast) or all clients (Broadcast).  
This is a simpler version of any commercial chat-based application which make use of centralized servers for relaying the messages, except the fact that messages would be encrypted in the case of commercial applications.

### Solution
The assignment is implemented using ```Python```. There are two files in the [src](./A2/src/) folder - ```server.py``` and ```client.py```. The first file contains the code for the server end whereas the second file contains the code for the client end.

How to run the code?
1. Run the following command to start the server-
    ```
    python3 server.py 
    ``` 
2. Run the following command to start a client system-
    ```
    python3 client.py [USERNAME] [SERVER_IP]
    ```
    Here, USERNAME is the client's username. This username can only contain alphanumeric ```[a-zA-Z1-9]``` characters.

    SERVER_IP is the server's IP address for connection. For example- ```127.0.0.1``` (localhost)

*Note*
- ```ALL``` can't be used as a client's username. It is a reserved keyword for broadcast messages. 
- Multiple clients can be started using the above mentioned command on same system.

Message format
- For unicast messages, use
    ```
    @[RECVR_USERNAME]: Message
    ```
    Remember RECVR_USERNAME should be a legal username of a client connected to the user. Otherwise, error would be raised by the server side (ERROR 102). \
     If the format of the message is wrong, appropiate help will be printed.

- For broadcast messages, use
    ```
    @ALL: Message
    ```
    Broadcast messages are visible to all the clients connected to the server. 

Implementation details
- ```Python``` is used to write the code. Python's ```socket``` library is used for making TCP sockets. 
- To handle multiple clients, threads are used to run parallel processes. Python's ```threading``` library is used for making threads.  
- At client's end, two threads are used to separate the sending and listening sockets' processes.
- Registration protocol is implemented to register client's sockets at server side. 


## Assignment 3

### Author
* Name : [Gaurav Jain](https://github.com/GAURAV-28/)
* Entry Number : 2019CS10349