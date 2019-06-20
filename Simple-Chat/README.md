# C-Networking
Communication Between Clients
You must compile Server.c at Linux subsystem on Windows or Linux.

For Server.c:
gcc Server.c -o Server -lpthread

For Client.c
gcc Client.c -o Client -lpthread

or

Compile it on compiler


For Executing Server.c: (For Windows, download ubuntu from Windows Store and use terminal)
./Server PORT_NAME

For Executing Client.c: (For Linux)
./Client PORT_NAME IP_ADRESS(Make it 127.0.0.1 if you run client and server on same computer)

For Windows users, run its executable file



