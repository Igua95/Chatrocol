#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define P_SIZE sizeof(struct pChatagram)

struct pChatagram {
	uint16_t code;
	uint16_t v2;
	char story[3][50];
	char message[50];
};

// Función que se encarga de leer un mensaje de aplicacion completo 
// lee exactamente la cantidad de bytes que se pasan en el argumento total:

int getChatagram ( int sd, char * buffer, int total ) {
    int bytes;
    int leido;

    leido = 0;
    bytes = 1;

    while ( (leido < total) && (bytes > 0) ) {

        bytes = recv ( sd , buffer + leido , total - leido , 0);

        leido = leido + bytes;

    }

    return ( leido );

}

int main(int argc, char *argv[]) {
	int n;
	int sd;
	int lon;
	char teclado[512];
	char buffer[P_SIZE];
	struct sockaddr_in servidor;
	struct sockaddr_in cliente;
	struct hostent *h;
	struct pChatagram *chatagram;

	if (argc < 2) {
		printf("Debe ejecutar %s (nombre de host)\n",argv[0]); 
		exit (-1);
	}

	sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(4446);
	//servidor.sin_addr.s_addr = inet_addr("x.x.x.x");

	if ( h = gethostbyname ( argv [1] ) ) {
		memcpy ( &servidor.sin_addr , h->h_addr , h->h_length );
        } else {
                herror("DNS error");
                exit (-1);
	}

	lon = sizeof(servidor);

	if ( connect ( sd , (struct sockaddr *) &servidor, lon ) < 0 ) {
		perror ("Error en connect");
		exit(-1);
	}

	chatagram = (struct pChatagram *) buffer;

	while(1) {
		printf("Yo:");
		fgets(teclado, sizeof(teclado), stdin);
		teclado[strlen(teclado) - 1] = '\0';

		char fullInput[50];
		strcpy(fullInput,teclado);
		
		/* get the first command */
		const char s[1] = " ";
		char *command;
		command = strtok(teclado, s);
		
		if(!strcmp(fullInput,"\0")) { 
			chatagram->code = 120;
		} else if (!strcmp(command,"/login\0")) {
			char username[10];
			strcpy(username, strchr(fullInput, ' '));
			chatagram->code = 100;
			strcpy(chatagram->message,username);
        } else if (!strcmp(command,"/whoIsOn\0")) {
			chatagram->code = 110;
		} else {	
			chatagram->code = 400;
			strcpy(chatagram->message, teclado);
		}
		

		send ( sd, buffer, P_SIZE, 0 );

		n = getChatagram (sd, buffer, P_SIZE );

		if(chatagram->code == 401) {
			size_t i = 0;
			for( i = 0; i < sizeof(chatagram->story) / sizeof(chatagram->story[0]); i++)
			{
				printf("%s\n", chatagram->story[i] );
			}
		} else {
			printf("Server dice: %s \n", chatagram->message);
		}


	}
	


	close(sd);

}