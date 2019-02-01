#include <stdio.h>
#include <string.h>

void string(int *pointer)
{
    *pointer = 2;
}
 
int main()
{
    int a;

    a = 1;

    string(&a);
    printf("The number of the variables is %d",a)
  return 0;
}