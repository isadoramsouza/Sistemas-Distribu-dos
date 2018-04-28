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


#define tam 30  //tamanho de msg


char endereco[9]="servidor1"; 
int sockfd;
int len;
struct sockaddr_un address;
int result;



int xcpu, ymem,x,y,tempo; 

void envia();
void trocaMsg(); 
void fazConexaoServidor();

//separa msg recebida pelo servidor, x eh a memoria e y cpu. Converte pra inteiro.
void separa(char resp[]){
	char *x,*y;
	x = strtok (resp, "#"); 
	y = strtok (NULL, "#");
	xcpu = atoi(x);
	ymem = atoi(y);
}


//envia mensagem de verificacao de espaco para o servidor e chama troca msg
//gera randomicamente mem, cpu e tempo para alocar. Caso tenha que digitar na mao basta trocar.
void envia(){
	char msgverificacao[tam]="#cpu?#mem?#";
	srand((unsigned)time(NULL));	
	x=rand()%90;	
	y=rand()%90;
	tempo=rand()%120000+10;
	
	printf("Cpu e memoria solicitadas: %d %d\n", x, y);
	printf("Tempo solicitado: %.2lf\n", (double)tempo*0.001);
	//printf("Digite a qtd de cpu e mem para alocar:\n");
	//scanf("%d",  &x);
	//scanf("%d",  &y);
	write(sockfd, msgverificacao, tam);
	trocaMsg();
}

//troca mensagem com o servidor enquando a resposta do mesmo for concedida, caso contrario muda servidor.
void trocaMsg(){
	
	char msgrespverif[tam];
	char msgalocacao[tam];
	int qtdcpu, qtdmem;
	char x1[2], y1[2], tempo1[10];
	char msgrespostaalocacao[tam];
	
	strcpy(msgrespostaalocacao,"#concedida#");
	while(strcmp(msgrespostaalocacao, "#concedida#")==0){
	memset(msgrespverif, 0, sizeof(msgrespverif));//limpa variavel
	read(sockfd, msgrespverif, tam); //recebe a resposta de verificacao do servidor
	if(strlen(msgrespverif)==0){ 
		close(sockfd);
		printf("Fechando conexao..\n\n");
		fazConexaoServidor();
	}else{
	printf("resposta servidor: %s\n", msgrespverif);
	
	separa(msgrespverif); //separa
	printf("total disponivel %d %d.\n", (xcpu), (ymem));
	if(xcpu==0 && ymem==0){ //caso receba tudo 0, servidor cheio.
		printf("servidor cheio.\n");
		
	}

	
	if(x<=(xcpu) && y<=(ymem)){ //se os numeros sorteados estiverem dentro do limite do servidor, envia msg de alocacao.
		sprintf (x1,"%d",x);
		sprintf (y1,"%d",y);
		sprintf (tempo1,"%d",tempo);
		strcpy(msgalocacao,"");
		strcat(msgalocacao, "#");
		strcat(msgalocacao, x1);
		strcat(msgalocacao, "cpu#");
		strcat(msgalocacao, y1);
		strcat(msgalocacao, "mem#");
		strcat(msgalocacao, tempo1);
		strcat(msgalocacao, "#");	

		write(sockfd, msgalocacao, tam);
		
		
		read(sockfd, msgrespostaalocacao, tam); //recebe resposta
			
		
		printf("resposta da alocacao: %s\n\n\n", msgrespostaalocacao);


	
		if(strcmp(msgrespostaalocacao, "#concedida#")==0){ // se concedida, dorme 10 segundos e chama rotina de novo no mesmo sv.
			sleep(10);
			envia();
			
		}
		if(strcmp(msgrespostaalocacao, "#negada#")==0){ //se negada, fecha conexao, dorme 10 segundos e troca de sv.
			
			close(sockfd);
			printf("Fechando conexao..\n\n");
			sleep(10);
			fazConexaoServidor();
			
		}
	
	}else{//se os numeros sorteados nao estiverem dentro do limite do servidor, fecha conexao, dorme 10 s e troca de sv.
		close(sockfd);
		printf("Ultrapassou o limite disponivel do servidor.\n");
		printf("Fechando conexao..\n\n");
		sleep(10);
		fazConexaoServidor();		
	}
	
}}}



void fazConexaoServidor(){    
	if(strcmp(endereco,"servidor1")==0){//inicia endereco como servidor 1 e testa cada vez que chama a funcao, mudando o valor.
		strcpy(endereco,"servidor2"); //conecta primeiro no 2.
	}else{
		strcpy(endereco,"servidor1");
	}
		
	printf("Conectando no: %s\n", endereco);
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, endereco);
	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *)&address, len);	
	if(result == -1) {
		perror("oops: client1");	
		exit(1);
}
	envia();
}




int main (){
	
	fazConexaoServidor();	
	
}


