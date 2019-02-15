/* Example using strcat by TechOnTheNet.com */

#include <stdio.h>
#include <string.h>

int main(int argc, const char * argv[])
{
    char teclado[512];

    while(1) {
		fgets(teclado, sizeof(teclado), stdin);
		teclado[strlen(teclado) - 1] = '\0';

		char fullInput[50];
		strcpy(fullInput,teclado);
		
		/* get the first command */
		const char s[1] = " ";
		char *command;
		command = strtok(teclado, s);    

        printf("Commando es: %s",command);
        
    }
    

   return 0;
}