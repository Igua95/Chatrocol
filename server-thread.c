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

void *suma (void *);
int leer_mensaje ( int  , char * , int );

#define P_SIZE sizeof(struct psuma)

struct psuma {
	uint16_t v1;
	uint16_t v2;
	char msj[256];
};

struct args {
    char mensaje[256];
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
	servidor.sin_port = htons (4444);
	servidor.sin_addr.s_addr = INADDR_ANY;

	sd = socket (PF_INET, SOCK_STREAM, 0);

	if ( bind ( sd , (struct sockaddr *) &servidor , sizeof(servidor) ) < 0 ) {

		perror("Error en bind");
		exit (-1);

	}

	listen ( sd , 5);

	strcpy(arguments->mensaje,"Mensaje en blanco");


	while (1) {

		lon = sizeof(cliente);

		sd_cli = accept ( sd , (struct sockaddr *) &cliente , &lon);
		arguments->socket_client = sd_cli;

		pthread_create ( &tid, NULL, suma, (void *) arguments );

	}

	close (sd);

}

void *suma ( void *arg ) {

	struct args *argumentos = arg;


	
	printf("Mensaje: %s\n", argumentos->mensaje);
    printf("Socket: %d\n", ((struct args*)arg)->socket_client);

	int sdc;
	int n;
	char buffer[P_SIZE];
	struct psuma *suma;

	char msjAux[256];

	suma = (struct psuma *) buffer;

    sdc = ((struct args*)arg)->socket_client;

	n = 1;
	while ( n != 0) {

		if ( ( n = leer_mensaje ( sdc , buffer , P_SIZE ) ) > 0 ) {

			// suma->msj = htonl ( ntohs (suma->v1) + ntohs(suma->v2) );
			printf("recibi: %s \n", suma->msj);
			strcpy(msjAux,suma->msj);
			strcpy(suma->msj,argumentos->mensaje);

			send ( sdc , buffer , P_SIZE ,0 );
			strcpy(argumentos->mensaje,msjAux);
		}

	}
	close (sdc);
	
}

int leer_mensaje ( int socket , char *buffer , int total ) {

	int bytes, leido;

	leido = 0;
	bytes = 1;
	while ( (leido < total) && (bytes > 0) ) {
		bytes = recv ( socket , buffer + leido , total - leido , 0 );
		leido = leido + bytes;
	}
	return (leido);
}
