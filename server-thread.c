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
	servidor.sin_port = htons (4446);
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
			if(users[i].socketAssigned == 0) {
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


	
	printf("Mensaje: Re locooooo %s\n", argumentos->mensaje[0]);
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

			printf("recibi el codigoooo: %d \n", chatagram->code);

			if(120 == chatagram->code) {
				printf("Solicita refresh \n");
			}


			if(chatagram->code == 100) {
				int k = 0;
				int encontrado = 0;
				chatagram->code = 101; // no hay lugar disponible
				while(!encontrado && k < sizeof(users) / sizeof(users[0])) {
					if(users[k].socketAssigned == sdc) {
						encontrado = 1;
						strcpy(users[k].nick,chatagram->message);
						chatagram->code = 102;
						strcpy(chatagram->message,"Usuario registrado!");
					}
					k++;
				}
				printf("Usuario: %d %s \n",users[0].socketAssigned, users[0].nick);
				printf("Usuario: %d %s \n",users[1].socketAssigned, users[1].nick);
				printf("Usuario: %d %s \n",users[2].socketAssigned, users[2].nick);
			} else if (chatagram->code == 120) {
				chatagram->code=121;
				break;
			} else {
				chatagram->code=401;
				updateStories(argumentos,chatagram->message);
			}
			

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
