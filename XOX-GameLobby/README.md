In XOX-GameLobby, each 2 client matchs in lobbies. When one client makes a move, it sends a data that represents its move data
and only its opponent client takes it and client updates each client's game board according to move data. If a player disconnects,
Lobby will be empty by server so opponent disconnected from lobby too.

How to Run?

For Windows:
  Compile Client.c with gcc by adding compiler to these commands: -lwsock32 -lws2_32
  Compile Server.c by an Ubuntu Console with: gcc Server.c -o Server -lpthread

  Run Client.exe directly, Run Server with ./Server 8888 (port is setted as 8888 in Client so use 8888)
