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

int fdSocket, mainSocket;
struct sockaddr_in socketAddr, socketAddr2;
	socklen_t sockLen;
char endServidor1[100] ="localhost",endServidor2[100]="localhost";
char endereco[100]="localhost";
int xcpu, ymem,x,y;

void envia();
void trocaMsg(); 
void fazConexaoServidor();
void separa(char resp[]){
	char *x,*y;
	x = strtok (resp, "#");
	y = strtok (NULL, "#");
	xcpu = atoi(x);
	ymem = atoi(y);
}

void envia(){
	char msgverificacao[30]="#cpu?#mem?#";
	srand((unsigned)time(NULL));	
	//x=rand()%90;	
	//y=rand()%90;
	//x=15;
	//y=15;
	
	printf("Digite a qtd de cpu e mem para alocar:\n");
	scanf("%d",  &x);
	scanf("%d",  &y);
	if(send(fdSocket, msgverificacao,30, 0) < 0){
		perror("Unable to send message\n");	
	}
	trocaMsg();
}


void trocaMsg(){
	
	char msgrespverif[30];
	char msgalocacao[30];
	int qtdcpu, qtdmem;
	char x1[2], y1[2];
	char msgrespostaalocacao[30];
	
	strcpy(msgrespostaalocacao,"#concedida#");
	while(strcmp(msgrespostaalocacao, "#concedida#")==0){
	if(recv(fdSocket, msgrespverif ,30, 0) < 0){
		perror("Unable to receive message\n");
	}
	printf("resposta servidor: %s\n", msgrespverif);
	
	separa(msgrespverif);
	printf("total disponivel %d %d.\n", (100-xcpu), (100-ymem));
	if(xcpu==100 && ymem==100){
		printf("servidor cheio.\n");
	}

	
	if(x<=(100-xcpu) && y<=(100-ymem)){
		
		sprintf (x1,"%d",x);
		sprintf (y1,"%d",y);
		strcpy(msgalocacao,"");
		strcat(msgalocacao, "#");
		strcat(msgalocacao, x1);
		strcat(msgalocacao, "cpu#");
		strcat(msgalocacao, y1);
		strcat(msgalocacao, "mem#10#");	

		if(send(fdSocket, msgalocacao,30, 0) < 0){
			perror("Unable to send message\n");	
		}	
		if(recv(fdSocket, msgrespostaalocacao ,30, 0) < 0){
			perror("Unable to receive message\n");
		}
		printf("resposta da alocacao: %s\n\n\n", msgrespostaalocacao);

		///////////////////////////////////////////////////////////////
	
		if(strcmp(msgrespostaalocacao, "#concedida#")==0){
			//sleep(5);
			envia();
			
		}
		if(strcmp(msgrespostaalocacao, "#negada#")==0){
			//close(mainSocket);
			printf("Fechando conexao..\n\n");
			fazConexaoServidor();
			
		}
	
	}else{
		//close(mainSocket);
		printf("ultrapassou o limite disponivel do servidor.\n");
		("Fechando conexao..\n\n");
		fazConexaoServidor();		
	}
	
}}



void fazConexaoServidor(){    
	if(strcmp(endereco,endServidor1)==0){
		strcpy(endereco,endServidor2);
	}else{
		strcpy(endereco,endServidor1);
	}
		if((fdSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			perror("Unable to create socket\n");
			printf("Exit %s:%d\n", __FILE__, __LINE__);
			exit(1);
		}

		//Setting socket parameters
		memset(&socketAddr, 0,sizeof(socketAddr));
	     	socketAddr.sin_family = AF_INET;	
	     	socketAddr.sin_port   = htons(PORT);
	     	inet_aton(endereco, &(socketAddr.sin_addr));	//Set the destination address
		
		printf("Vou conectar.\n");

		//Connect to a socket
    		if(connect(fdSocket, (struct sockaddr *) &socketAddr, sizeof(socketAddr)) < 0){
			printf("ERRO");
		}
	envia();
}




int main (){
	
	fazConexaoServidor();	
	
}


