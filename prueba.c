#include <stdio.h>
#include <string.h>

int main () {
   const char str[] = "http:/ /www.tutorialspoint.com";
   char *ret;

   ret = strchr(str, '.');

   printf("String after is - |%s|\n", ret);
   
   return(0);
}