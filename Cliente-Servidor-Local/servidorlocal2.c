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
#define tam 30


int server_sockfd, client_sockfd[10000];
int server_len, client_len;
struct sockaddr_un server_address;
struct sockaddr_un client_address;


int minhacpu = 100;
int minhamem = 100;
int xcpu, ymem, tempo; 
int cpudisp, memdisp;

//copia o tempo, mem e cpu enviado pelo cliente para variavel local, dorme e atualiza mem e cpu.
void *desaloca(){
		double t=(double)tempo*0.001;
		
		int x,y;
		x=xcpu;
		y=ymem;
		sleep(t);
		printf("desalocando.\n");
		cpudisp=cpudisp+x;
		memdisp=memdisp+y;
		
		printf("total disponivel cpu %d, mem %d\n\n", cpudisp, memdisp);

}

//separa msg de alocacao. Mem, cpu e tempo. Converte para inteiro.
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
	char msgverificacao[tam];
	char msgrespostaverif[tam];
	char verfcpu[2], verfmem[2];
	char msgalocacao[tam];
	char msgrespostaalocacao[tam];
	long id=(long)ID;
	int j=0;

	
	while(1){
	memset(msgverificacao, 0, sizeof(msgverificacao));
	read(client_sockfd[id], msgverificacao, tam); //recebe msg de verificacao
	
	if(strlen(msgverificacao)!=0){
	printf("recebi do cliente: %s\n\n", msgverificacao);

	sprintf (verfcpu,"%d",cpudisp);
	sprintf (verfmem,"%d",memdisp);
	strcpy(msgrespostaverif,"");
	strcat(msgrespostaverif, "#");
	strcat(msgrespostaverif,verfcpu);
	strcat(msgrespostaverif, "cpu#");
	strcat(msgrespostaverif, verfmem);
	strcat(msgrespostaverif, "mem#");

	
	write(client_sockfd[id], msgrespostaverif, tam); //envia resposta com mem e cpu livres
	

	memset(msgalocacao, 0, sizeof(msgalocacao));
	read(client_sockfd[id], msgalocacao, tam); //recebe msg de alocacao

	
	if(strlen(msgalocacao)!=0){
	printf("recebi do cliente: %s\n\n", msgalocacao);
	separa(msgalocacao); 

	if(xcpu<=(cpudisp) && ymem<=(memdisp)){ //se estiver dentro do limite, aloca
		printf("Alocando por  %.2lf.\n", ((double)tempo*0.001));
		cpudisp=cpudisp-xcpu;
		memdisp=memdisp-ymem;	
		printf("total cpu %d, mem %d\n", cpudisp, memdisp);
		strcpy(msgrespostaalocacao,"#concedida#");
		printf("Ha espaco\n\n");
		write(client_sockfd[id], msgrespostaalocacao, tam); //envia resposta concedida
	pthread_create(&t[j],NULL,&desaloca,0); //chama thread desaloca
	j++;

	}else{ //se n estiver dentro do limite, envia resposta negada e fecha conexao.
		strcpy(msgrespostaalocacao,"#negada#");
		printf("Nao ha espaco\n\n");
		write(client_sockfd[id], msgrespostaalocacao, tam);
		close(client_sockfd[id]); 
		pthread_exit(NULL);
		
	}

	
	}else{ //se n receber nada do cliente apos ele fechar, fecha tb.
	printf("Cliente desconectou.\n\n");
	close(client_sockfd[id]);
	pthread_exit(NULL);
	}
}}}


void *aceitaConexao () {
	pthread_t t[10000];	
	int j=0,numcon=0, i=0;
	long id;

	server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	server_address.sun_family = AF_UNIX;
	strcpy(server_address.sun_path, "servidor2");
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
	listen(server_sockfd, 5);
	printf("Aguardando conexão.\n");
	while(1) {
		
		client_len = sizeof(client_address);
		client_sockfd[i] = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);
		id=i;
		printf("Cliente conectou.\n");
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

	cpudisp=minhacpu*0.9; //capacidade inicial = 90
	memdisp=minhamem*0.9;
	pthread_create(&thread[1],NULL,&aceitaConexao,0);
	pthread_join(thread[1],NULL);	
}
