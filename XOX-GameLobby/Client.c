
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef __WIN32__
	#include <winsock2.h>
	#include <windows.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")
	#pragma comment (lib, "Mswsock.lib")
#else
	#include <sys/socket.h>
	#include <arpa/inet.h> //Ýnternet adresi oluþturmak için gerekli
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#define YES 1
#define NO 0
#define UP 0x48
#define DOWN 0x50
#define LEFT 0x4D
#define RIGHT 0x4B

#define PORT 8888
#define LOCALHOST "127.0.0.1"
#define IP LOCALHOST
typedef struct{
	int client;
	char buff[255];
	
}PARAM;
typedef struct{
	int x;
	int y;
	char a;
}DATA;

int read_in(int,char*, int);
void updateBoard();
void printBoard();
void selectt();
void cleanSelect();
int isFinished();
char Board[6][3]={"___","   ","___","   ","___","   "};
DATA* data;
int sirasende=NO;
int main(/*int argc ,char* argv[]*/){
	
		/*if(argc > 2) {
		printf("Cok fazla parametre girdiniz");
		exit(1);
	}
	int arg =atoi(argv[1]);*/
	#ifdef __WIN32__
		WSADATA wsaData;
		int iResult;
	
	// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) {
	    	printf("WSAStartup failed: %d\n", iResult);
	    	return 1;
		}
	#endif
	data = (DATA*)malloc(sizeof(DATA));
	char buff[255];
	char key;
	struct sockaddr_in serv;
	int client_d = socket(AF_INET, SOCK_STREAM, 0);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT); //Port is Macro
	
	//Bind client to localhost (IP is Macro)
	inet_pton(AF_INET, IP, &serv.sin_addr); 

	//Connect client to server
	int f= connect(client_d, (struct sockaddr *)&serv, sizeof(serv)); 
	//Karþý taraftan gelenler okumaný saðlamak için while döngüsü
	if(f!=0)
		printf("Connected");
	char first[3];
	first[0]='A';
	while(first[0]=='A')
		read_in(client_d,first,sizeof(first));
		
	char flag[2]="00";
	printf("\nWaiting for opponent...");
	
	while(flag[0]=='0')
		read_in(client_d,flag,sizeof(flag));
	
	printf("\nOpponent connected!");
	data->a=first[0];
	first[1]-='0';
	sirasende=first[1];
	while(isFinished()==NO){
		printf("%d",isFinished());
		printBoard();
		if(sirasende==NO){
			
			//Read updated board
			printf("\nWaiting for Opponent's Move..\n");
			buff[0]='A';

			while(buff[0]=='A')
				read_in((client_d),buff,sizeof(buff));
			data->x=buff[0]-'0';
			data->y=buff[1]-'0';
			data->a=buff[2];
			//Update Board
			updateBoard();
			printBoard(); 
			//Empty buff array
			memset(buff,'\0',sizeof(buff));
			//Change turn
			sirasende=YES;
		}
		else{
			//Set x and y data for select cursor
			data->x=0;
			data->y=1;
			data->a=first[0];
			//Store input at key variable
			key='0';
			//Clean board's select
			cleanSelect();
			selectt();
			printBoard();
			//Until key is Enter and entered grid is not X or O
			while(key!=13 || Board[data->y-1][data->x]!='_'){
				key = getch();
				switch(key){
					case UP:
						if(data->y>1){
							Board[data->y][data->x]=' ';
							data->y-=2;							
						}
						break;
					case DOWN:
						if(data->y<5){
							Board[data->y][data->x]=' ';							
							data->y+=2;
						}
						break;
					case LEFT:
						if(data->x<2){
							Board[data->y][data->x]=' ';							
							data->x++;
						}
						break;
					case RIGHT:
						if(data->x>0){
							Board[data->y][data->x]=' ';							
							data->x--;
						}
						break;
					default:
						break;
				}			
				//update Board's cursor
				selectt();
				//Print Board
				printBoard();
			}
			//Empty buff
			memset(buff,'\0',sizeof(buff));
			//Buff will be sended to other client
			buff[0]=data->x;
			buff[1]=--data->y; //we decrease it first because we want to send XOX position, not cursor location
			buff[2]=data->a; //X or O
			sprintf(buff,"%d%d%c",data->x,data->y,data->a);
			send(client_d,buff,255,0);
		//	fgets(buff,4,stdin); //BURADAN ALINCA OLUYOR AMA MANUEL ALINCAO LMUYOR BUNU ÇÖZ
			//sEND DATA;
		//	say(client_d,buff);
			//Change turn
			sirasende=NO;
			updateBoard();
		}
		
	}
	printf("\nGameOver\n");
	getchar();
	puts("Disconnected");
}
void updateBoard(){
	Board[data->y][data->x]=data->a;
}
void selectt(){
	Board[data->y][data->x]='_';
}
void printBoard(){
	int i,j;
	system("cls");
	printf("\n\n\n");
	for(i=0;i<6;i++){
		printf("\t\t");
		for(j=0;j<3;j++)
			printf("%c    ",Board[i][j]);
	printf("\n");
	}
}
void cleanSelect(){
	int i,j;
	for(i=1;i<6;i+=2){
		for(j=0;j<3;j++)
			Board[i][j]=' ';
	}
}
int isFinished(){
	int i,j;
	for(i=0;i<6;i+=2){
		if(Board[i][2]!='_' && Board[i][0]==Board[i][1] && Board[i][1]==Board[i][2])
			return 1;
	}
	for(j=0;j<2;j++)
		if(Board[2][j]!='_' && Board[0][j]==Board[1][j] && Board[1][j]==Board[2][j])
			return 1;

	return 	(Board[0][0]!='_' && Board[0][0]==Board[2][1] && Board[2][1]==Board[4][2]) || (Board[4][0]!='_' && Board[4][0]==Board[2][1] && Board[2][1]==Board[0][2]);
}
int read_in(int socket, char *buf, int len) { 
	char *s = buf;
	int slen = len;
	int c = recv(socket, s, slen, 0);
	if(c>0)
		return c;
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

