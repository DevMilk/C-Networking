#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h> 

typedef struct{
	char buff[255];
	int cl1;
	int cl2;
}PARAM;
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
			fprintf(stderr,"%s: %s\n","Client'e konusmakta sorun var",strerror(errno));
			//Bir problem varsa error() çaðýrma. Sýrf 1 client için serveri durdurmak yanlýþ.

	return result;
}
int s_bind(int socket, int port){
	struct sockaddr_in a;
	a.sin_family = AF_INET;
	a.sin_port = (in_port_t)htons(port); 
	a.sin_addr.s_addr=inet_addr("127.0.0.1");
	int reuse=1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1) 
		 perror("Can't set the reuse option on the socket");
	if(bind(socket, (struct sockaddr*) &a,sizeof(a))==-1)
		perror("Bind islemi basarisiz");
} 

void *client_say(void *args){
//	char buff[255];
	int cl1 =(int)((PARAM*)args)->cl1;
	int cl2 =(int)((PARAM*)args)->cl2;
	while(1){

	memset(((PARAM*)args)->buff,0,255);
	say(cl1," ");
	read_in(cl1,((PARAM*)args)->buff,255);
	say(cl2,"Client1> ");
	say(cl2,(((PARAM*)args)->buff));
	say(cl2,"\n");
}
}
int main(int argc ,char* argv[]){
	char buff[255];
//	struct sockaddr_in serveradres;
	struct sockaddr_storage serverdepo;
	socklen_t adressize=sizeof(serverdepo);

/*	serveradres.sin_family = AF_INET;
	serveradres.sin_port = (in_port_t)htons(30000); 
	serveradres.sin_addr.s_addr=inet_addr("127.0.0.1");
	memset(serveradres.sin_zero,'\0',sizeof(serveradres.sin_zero));*/
	
	int server,client1,client2;
	server = socket(AF_INET,SOCK_STREAM,0);
		s_bind(server, 30000);

	if(listen(server,2)==-1)
		puts("Baglanmada hata var");
		
	PARAM *parametreler=(PARAM*)malloc(sizeof(PARAM)); //={client1,client2};
	puts("Ilk client bekleniyor...");
	client1=accept(server,(struct sockaddr*)&serverdepo,&adressize);
	puts("Ilk client baglandi");
			say(client1,"deneme123");

	puts("2. client bekleniyor...");
	client2 = accept(server, (struct sockaddr*)&serverdepo,&adressize);
	puts("2. client baglandi.");
	parametreler->cl1= client1;
	parametreler->cl2= client2;	
	int a=0;
	pthread_t thr;
	puts("Thread olusturuldu.");
    pthread_create(&thr,NULL,client_say,(void*)parametreler);
	
	
	while(1){		//PTHREAD'E PARAMETRE GÖNDERMEK SORUN
		read_in(client2,buff,255);
		say(client1,"Client2> ");
		say(client1,buff);
		say(client1,"\n");
	//	pthread_join(thr,NULL);
		puts("\n");
		puts("Client2deki buff: ");
		puts(buff);
		puts("\n");
	}	
	
	
	return 0;
}
