#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

// Con el siguiente protocolo
//  request cliente                   response servidor
//  Cod: 100 (login)                  -> 101 Registered 102 no registered 
//  Cod: 110 (whoIsOn)                -> 111-ConnectUser[], 112 no hay nadie
//  Cod: 120 (refresh)                -> 121-Cola
//  Cod: 130 quit					  -> x
//  Cod: 140 new message              -> 141 gotcha

void *clientDispacher (void *);
void updateStories (int *, char [50], int);
void login(int * , int);
int leer_mensaje ( int  , char * , int );
void getUsersOnline(int *);
void cleanBuffer(int *);
void quitter(int);

#define P_SIZE sizeof(struct pChatagram)

struct pChatagram {
	uint16_t code;
	uint16_t v2;
	char story[3][50];
	char message[50];
};
struct usersOnline {
	char nick[10];
	int socketAssigned;
} ;

struct usersOnline users[3];

struct args {
    char mensaje[3][50];
    int socket_client;
};


int main() {
	int lon;
	int sd;
	int sd_cli;
	struct sockaddr_in servidor;
	struct sockaddr_in cliente;
	pthread_t tid;
	
    struct args *arguments = (struct args *)malloc(sizeof(struct args));
	
	servidor.sin_family = AF_INET;
	servidor.sin_port = htons (4445);
	servidor.sin_addr.s_addr = INADDR_ANY;

	sd = socket (PF_INET, SOCK_STREAM, 0);

	if ( bind ( sd , (struct sockaddr *) &servidor , sizeof(servidor) ) < 0 ) {

		perror("Error en bind");
		exit (-1);

	}

	listen ( sd , 5);

	while (1) {

		lon = sizeof(cliente);

		sd_cli = accept ( sd , (struct sockaddr *) &cliente , &lon);
		arguments->socket_client = sd_cli;

		int i = 0;
		int encontrado = 0;
		while(!encontrado && i < sizeof(users) / sizeof(users[0])) {

			if(users[i].socketAssigned == sd_cli ) {
				encontrado = 1;
			}
			
			if(users[i].socketAssigned == 0 ) {
				encontrado = 1;
				users[i].socketAssigned = sd_cli;
			}
			i++;
		}

		pthread_create ( &tid, NULL, clientDispacher, (void *) arguments );

	}

	close (sd);

}

void *clientDispacher ( void *arg ) {

	struct args *argumentos = arg;
	int sdc;
	int n;
	char buffer[P_SIZE];
	struct pChatagram *chatagram;

	char msjAux[50];

	chatagram = (struct pChatagram *) buffer;

    sdc = ((struct args*)arg)->socket_client;

	n = 1;
	while ( n != 0) {

		if ( ( n = getChatagram ( sdc , buffer , P_SIZE ) ) > 0 ) {

			printf("Recibo codigo: %d \n", chatagram->code);


			int k = 0;
			int encontrado = 0;

			if(chatagram->code == 100) {
				// Register
				login(chatagram, sdc);
			} else if (chatagram->code == 110) {
				//Who is online
				getUsersOnline(chatagram);
			} else if (chatagram->code == 120) {
				// Refresh
				strcpy(chatagram->story[0],argumentos->mensaje[0]);
				strcpy(chatagram->story[1],argumentos->mensaje[1]);
				strcpy(chatagram->story[2],argumentos->mensaje[2]);
				chatagram->code=121;
			} else if (chatagram->code == 130) {
				quitter(sdc);
				chatagram->code=131;
				strcpy(chatagram->message,"Usuario desconectado!");
			} else {
				//new message
				updateStories(argumentos,chatagram->message, sdc);
				strcpy(chatagram->story[0],argumentos->mensaje[0]);
				strcpy(chatagram->story[1],argumentos->mensaje[1]);
				strcpy(chatagram->story[2],argumentos->mensaje[2]);
				chatagram->code=141;
			}
			



			printf("Envio codigo %d \n",chatagram->code);
			send ( sdc , buffer , P_SIZE ,0 );
		}
		cleanBuffer(chatagram);

	}
	close (sdc);
	
}

void updateStories(int * args, char message[50], int sdc) {
	struct args * argumentos = args;
	char fullMessage[50];
	int k = 0;
	int encontrado = 0;

	// obtengo el username
	while(!encontrado && k < sizeof(users) / sizeof(users[0])) {
		if(users[k].socketAssigned == sdc) {
			encontrado = 1;
			strcpy(fullMessage,users[k].nick);
		}
		k++;
	}
	strcat(fullMessage, ": ");
	strcat(fullMessage, message);

	strcpy(argumentos->mensaje[0],argumentos->mensaje[1]);
	strcpy(argumentos->mensaje[1],argumentos->mensaje[2]);
	strcpy(argumentos->mensaje[2],fullMessage);
}

int getChatagram ( int socket , char *buffer , int total ) {

	int bytes, leido;

	leido = 0;
	bytes = 1;
	while ( (leido < total) && (bytes > 0) ) {
		bytes = recv ( socket , buffer + leido , total - leido , 0 );
		leido = leido + bytes;
	}
	return (leido);
}

void login(int * pChatagram, int sdc) {
	struct pChatagram * chatagram = pChatagram;

	chatagram->code = 102; // no hay lugar disponible
	int k = 0;
	int encontrado = 0;
	while(!encontrado && k < sizeof(users) / sizeof(users[0])) {
		if(users[k].socketAssigned == sdc) {
			encontrado = 1;
			strcpy(users[k].nick,chatagram->message);
			chatagram->code = 101;
			strcpy(chatagram->message,"Usuario registrado!");
		}
		k++;
	}
}

void getUsersOnline(int * pChatagram) {
	struct pChatagram * chatagram = pChatagram;
	
	cleanBuffer(chatagram);
	//Valor por defecto "sin usuarios"
	chatagram->code = 112;

	int k = 0;
	while(k < sizeof(users) / sizeof(users[0])) {
		if(users[k].socketAssigned != 0) {
			strcpy(chatagram->story[k],users[k].nick);
			chatagram->code = 111;
		}
		k++;
	}
}

void cleanBuffer(int * pChatagram) {
	struct pChatagram * chatagram = pChatagram;

	size_t i = 0;
	for( i = 0; i < sizeof(chatagram->story) / sizeof(chatagram->story[0]); i++)
	{
		strcpy(chatagram->story[i],"");
	}
	chatagram->code = 0;
	strcpy(chatagram->message,"");
}

void quitter(int sdc) {
	int k = 0;
	int encontrado = 0;
	while(!encontrado && k < sizeof(users) / sizeof(users[0])) {
		if(users[k].socketAssigned == sdc) {
			encontrado = 1;
			users[k].socketAssigned = 0;
			strcpy(users[k].nick,"");
		}

		k++;
	}
}
