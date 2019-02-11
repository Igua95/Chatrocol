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

// COMPILAR: cc -o ser tcp_server_threads.c -l pthread

void *clientDispacher (void *);
void updateStories (int *, char [50]);
int leer_mensaje ( int  , char * , int );

#define P_SIZE sizeof(struct pChatagram)

struct pChatagram {
	uint16_t v1;
	uint16_t v2;
	char story[3][50];
	char message[50];
};

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
	servidor.sin_port = htons (4447);
	servidor.sin_addr.s_addr = INADDR_ANY;

	sd = socket (PF_INET, SOCK_STREAM, 0);

	if ( bind ( sd , (struct sockaddr *) &servidor , sizeof(servidor) ) < 0 ) {

		perror("Error en bind");
		exit (-1);

	}

	listen ( sd , 5);

	strcpy(arguments->mensaje[0],"Mensaje en blanco");


	while (1) {

		lon = sizeof(cliente);

		sd_cli = accept ( sd , (struct sockaddr *) &cliente , &lon);
		arguments->socket_client = sd_cli;

		pthread_create ( &tid, NULL, clientDispacher, (void *) arguments );

	}

	close (sd);

}

void *clientDispacher ( void *arg ) {

	struct args *argumentos = arg;


	
	printf("Mensaje: %s\n", argumentos->mensaje[0]);
    printf("Socket: %d\n", ((struct args*)arg)->socket_client);

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

			printf("recibi: %s \n", chatagram->message);
			updateStories(argumentos,chatagram->message);
			// strcpy(argumentos->mensaje[0],argumentos->mensaje[1]);
			// strcpy(argumentos->mensaje[1],argumentos->mensaje[2]);
			// strcpy(argumentos->mensaje[2],chatagram->message);


			strcpy(chatagram->story[0],argumentos->mensaje[0]);
			strcpy(chatagram->story[1],argumentos->mensaje[1]);
			strcpy(chatagram->story[2],argumentos->mensaje[2]);

			send ( sdc , buffer , P_SIZE ,0 );
		}

	}
	close (sdc);
	
}

void updateStories(int * args, char message[50]) {
	struct args * argumentos = args;
	strcpy(argumentos->mensaje[0],argumentos->mensaje[1]);
	strcpy(argumentos->mensaje[1],argumentos->mensaje[2]);
	strcpy(argumentos->mensaje[2],message);
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
