#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h> //İnternet adresi oluşturmak için gerekli
typedef struct{
	int client;
	char buff[255];
	
}PARAM;
int say(int socket, char* s){
	int result = send(socket,s,strlen(s),0);
	if(result==-1)
			fprintf(stderr,"%s: %s\n","Client'e konusmakta sorun var",strerror(errno));
			//Bir problem varsa error() çağırma. Sırf 1 client için serveri durdurmak yanlış.

	return result;
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

void *yaz_t(void* args){
	while(1){
		memset(((PARAM*)args)->buff,'\0',sizeof(((PARAM*)args)->buff));
		fgets(((PARAM*)args)->buff,255,stdin);
		say(((PARAM*)args)->client,((PARAM*)args)->buff);
	}
}
int main(int argc ,char* argv[]){
	
		if(argc > 3) {
		printf("Cok fazla parametre girdiniz");
		exit(1);
	}
	int arg =atoi(argv[1]);
	char buff[255];
	struct sockaddr_in serv;
	int client_d = socket(AF_INET, SOCK_STREAM, 0);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(arg);
	inet_pton(AF_INET, argv[2], &serv.sin_addr); //This binds the client to localhost
	connect(client_d, (struct sockaddr *)&serv, sizeof(serv)); //This connects the client to the server.
	pthread_t yaz;
	PARAM* parametreler=(PARAM*)malloc(sizeof(PARAM));
 	parametreler->client=client_d;
 	
	pthread_create(&yaz,NULL,yaz_t,parametreler); //Yazmayı sağlayan fonksiyon
	int n=1;
	//Karşı taraftan gelenler okumanı sağlamak için while döngüsü
	while(read_in((client_d),buff,255)>0) {
   //	 n=read_in(client_d,buff,255);
   	 puts(buff);
   	 memset(buff,'\0',sizeof(buff));
    //An extra breaking condition can be added here (to terminate the while loop)
	}
	puts("Server ile baglanti kesildi.");
}
