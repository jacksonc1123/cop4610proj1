#include <stdio.h>
#include <sys/types.h>
/* #include <sys/wait.h> */
#include <unistd.h>
#include <string.h>


int main(void)
{
  /* pid_t ch; */
  char prompt[512];

  do {
    printf("prompt> ");
    scanf("%s", prompt);
  } while(strcmp(prompt,"quit"));

  return 0;	  
}
