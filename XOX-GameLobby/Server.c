#include <sys/types.h>
/*#ifdef __WIN32__
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")
#endif*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_LOBBY_COUNT 10

//YAPILACAKLAR: Lobide rakip geldiðinde veri göndererek rakibin geldiðini onaylama
//			 	
typedef struct{
	int players[2];
}Lobby;

struct client_info {
	int sockno;
	char ip[INET_ADDRSTRLEN];
};

Lobby* lobbies[MAX_LOBBY_COUNT];
int clients[21];
int n = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int LobbyCount;


int say(int,char*);
int read_in(int,char*,int);
Lobby* emptyLobb();
void sendToOpponent(char*,int);
void *recvmg(void*);

int main(int argc,char *argv[])
{
	LobbyCount=0;
	int i;
	for(i=0;i<MAX_LOBBY_COUNT;i++)
		lobbies[i]=(Lobby*)malloc(sizeof(Lobby));
	
	
	struct sockaddr_in my_addr,their_addr;
	int my_sock;
	int their_sock;
	socklen_t their_addr_size;
	int portno;
	pthread_t sendt,recvt;
	char msg[3];
	int len;
	struct client_info cl;
	char ip[INET_ADDRSTRLEN];;
	
	if(argc > 2) {
		printf("Too much parameters emtered");
		exit(1);
	}
	portno = atoi(argv[1]);
	my_sock = socket(AF_INET,SOCK_STREAM,0);
	memset(my_addr.sin_zero,'\0',sizeof(my_addr.sin_zero));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(portno);
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	their_addr_size = sizeof(their_addr);

	if(bind(my_sock,(struct sockaddr *)&my_addr,sizeof(my_addr)) != 0) {
		perror("Bind failed");
		exit(1);
	}	
	puts("Listening...");
	if(listen(my_sock,5) != 0) {
		perror("Listening failed");
		exit(1);
	}
	Lobby* ptr=(Lobby*)malloc(sizeof(Lobby));
	while(1) {
		if((their_sock = accept(my_sock,(struct sockaddr *)&their_addr,&their_addr_size)) < 0) {
			perror("Accept not successfull");
			exit(1);
		}
		
		pthread_mutex_lock(&mutex);
		inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
		printf("%s connected\n",ip);
		cl.sockno = their_sock;
		strcpy(cl.ip,ip);
		clients[n] = their_sock;
		
		ptr = emptyLobb();
		
		//Connect players to rooms
		if(ptr!=NULL){
			if(ptr->players[0]=='\0'){
				ptr->players[0]=their_sock;
				say(their_sock,"X1");
			}
			else{
				ptr->players[1]=their_sock;
				say(their_sock,"O0");
				//Start data for second player connected
				say(ptr->players[0],"1");
				say(ptr->players[1],"1");

			}
			
		}
		else
			say(their_sock,"Lobbies are full!");
		
		printf("hobbala\n");
		n++;
		pthread_create(&recvt,NULL,recvmg,&cl);
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}
void *recvmg(void *sock){
	struct client_info cl = *((struct client_info *)sock);
	char msg[10] ;
	char msn[10];
	int len;
	int i;
	int j;
	while((len = recv(cl.sockno,msg,10,0)) > 0) {
//		sprintf(msn,"\nClient%d> ",cl.sockno);
		msg[len] = '\0';
		strcat(msn,msg);
		sendToOpponent(msn,cl.sockno);
		memset(msg,'\0',sizeof(msg));
		memset(msn,'\0',sizeof(msn));
	}
	pthread_mutex_lock(&mutex);
	printf("%s is disconnected\n",cl.ip);
	for(i = 0; i < n; i++) {
		if(clients[i] == cl.sockno) {
			j = i;
			while(j < n-1) {
				clients[j] = clients[j+1];
				j++;
			}
		}
	}
	for(i=0;i<MAX_LOBBY_COUNT;i++){
		//Empty lobby if a client disconnected
		if(lobbies[i]->players[0]==cl.sockno || lobbies[i]->players[1]==cl.sockno){
			lobbies[i]->players[0]='\0';
			lobbies[i]->players[1]='\0';
		}
	}
	n--;
	pthread_mutex_unlock(&mutex);
}
void sendToOpponent(char *msg,int curr){
	int i=0;
	pthread_mutex_lock(&mutex);
	while(i<MAX_LOBBY_COUNT) {
		if(lobbies[i]->players[0]==curr || lobbies[i]->players[1]==curr) {
			if(send(curr==lobbies[i]->players[0] ? lobbies[i]->players[1]: lobbies[i]->players[0] ,msg,strlen(msg),0) < 0) {
				perror("Error in sending");
				
			}
		}
		i++;
	}
	pthread_mutex_unlock(&mutex);
}
Lobby* emptyLobb(){
	int i;
	for(i=0;i<MAX_LOBBY_COUNT;i++)
		if(lobbies[i]->players[0]=='\0' || lobbies[i]->players[1]=='\0'){
			return lobbies[i];
		}
	return NULL;
}
int read_in(int socket, char *buf, int len) { 
	char *s = buf;
	int slen = len;
	int c = recv(socket, s, slen, 0);
    while ((c > 0) && (s[c-1] != '\n')) {
      s += c;
	  slen -= c;
      c = recv(socket, s, slen, 0);
   }
     
  if (c < 0)
	return c;
  else if (c == 0)
    buf[0] = '\0';
  else
    s[c-1]='\0';
  return len - slen; 
}
int say(int socket, char* s){
	int result = send(socket,s,strlen(s),0);
	if(result==-1)
			fprintf(stderr,"%s: %s\n","Error in sending data to client",strerror(errno));
			//Bir problem varsa error() çaðýrma. Sýrf 1 client için serveri durdurmak yanlýþ.
	return result;

}

