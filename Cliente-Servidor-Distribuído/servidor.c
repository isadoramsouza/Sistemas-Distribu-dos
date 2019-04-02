#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include<time.h>
#include<string.h>
#define PORT 1212


int fdSocket[100], mainSocket;
	struct sockaddr_in socketAddr, socketAddr2;
	socklen_t sockLen;
int minhacpu = 10;
int minhamem = 10;
int xcpu, ymem, tempo;



void *desaloca(){
		int x,y;
		x=xcpu;
		y=ymem;
		sleep(5);
		printf("desalocando.\n");
		minhacpu=minhacpu-x;
		minhamem=minhamem-y;
		
		printf("total disponivel cpu %d, mem %d\n", minhacpu, minhamem);
}


void separa(char resp[]){
	char *x,*y,*z;
	x = strtok (resp, "#");
	y = strtok (NULL, "#");
	z = strtok (NULL, "#");
	xcpu = atoi(x);
	ymem = atoi(y);
	tempo = atoi(z);
}



void *trocaMsg(void *ID){
	pthread_t t[100];
	char msgverificacao[30];
	char msgrespostaverif[30];
	char verfcpu[2], verfmem[2];
	char msgalocacao[30];
	char msgrespostaalocacao[30];
	long id=(long)ID;
	int j=0;
	while(1){
	
	if(recv(fdSocket[id], msgverificacao ,30, 0) < 0){
		perror("Unable to receive message\n");
	}
	printf("recebi do cliente: %s\n", msgverificacao);
	/////////////////////////////////////////////////////////////////////	
	sprintf (verfcpu,"%d",minhacpu);
	sprintf (verfmem,"%d",minhamem);
	strcpy(msgrespostaverif,"");
	strcat(msgrespostaverif, "#");
	strcat(msgrespostaverif,verfcpu);
	strcat(msgrespostaverif, "cpu#");
	strcat(msgrespostaverif, verfmem);
	strcat(msgrespostaverif, "mem#");
	if(send(fdSocket[id], msgrespostaverif ,30, 0) < 0){
		perror("Unable to send message\n");
	}
	///////////////////////////////////////////////////////////////////////
	
	if(recv(fdSocket[id], msgalocacao ,30, 0) < 0){
		perror("Unable to receive message\n");
	}
	printf("recebi do cliente: %s\n", msgalocacao);
	separa(msgalocacao);
	//////////////////////////////////////////////////////////////////////
	if(xcpu<=(100-minhacpu) && ymem<=(100-minhamem)){
		minhacpu=minhacpu+xcpu;
		minhamem=minhamem+ymem;	
		printf("total cpu %d, mem %d\n", minhacpu, minhamem);
		strcpy(msgrespostaalocacao,"#concedida#");

		printf("Ha espaco\n\n");
		
	pthread_create(&t[j],NULL,&desaloca,0);
	j++;

	}else{
		strcpy(msgrespostaalocacao,"#negada#");
		printf("Nao ha espaco\n");

	}
	if(send(fdSocket[id], msgrespostaalocacao ,30, 0) < 0){
			perror("Unable to send message\n");
		}
	
	}
	
}






void abreconexao(){
	if((mainSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Unable to create socket\n");
		printf("Exit %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}
		//Setting socket parameters
	memset(&socketAddr, 0, sizeof(socketAddr));
	socketAddr.sin_family      = AF_INET;
	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	socketAddr.sin_port        = htons(PORT);   
	sockLen = sizeof(socketAddr);

		//Bind between socket and descriptor
	if(bind(mainSocket, (struct sockaddr *)&socketAddr, sockLen) < 0){
		perror("Unable to bind socket\n");
		printf("Exit %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}
		//Waiting for connections. The second parameter is the size of the queue for waiting connections
	if(listen(mainSocket, 100) < 0){
		perror("Unable to listen\n");
		printf("Exit %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}
}

void *aceitaConexao () {
	pthread_t t[100];	
	int j=0,numcon=0, i=0;
	long id;
	while(1){
		memset(&socketAddr2, 0, sizeof(socketAddr2));
		printf("Aguardando conexão.\n");
		//Accept connection on a socket
		fdSocket[i] = accept(mainSocket, (struct sockaddr *)&socketAddr2, &sockLen);
		id=i;
		printf("Conexão aceita.\n");
		numcon++;	
		pthread_create(&t[j],NULL,&trocaMsg,(void*)id);
				
		j++;
		i++;
	}
	for(j=0;j<numcon;j++){	
		pthread_join(t[j],NULL);
		
	}
}

		
int main () {
	pthread_t thread[1];
	int i,n;
	abreconexao();
	pthread_create(&thread[1],NULL,&aceitaConexao,0);
	pthread_join(thread[1],NULL);	
}
