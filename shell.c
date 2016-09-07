#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

char** parsecomm(char*);
int execute(char**);

int main(void)
{
  /* pid_t ch; */
  char prompt[512];
  /* char* argv[100]; */
  
  do {
    printf("prompt> ");
    fgets(prompt, 512, stdin);
    parsecomm(prompt);
  } while(strcmp(prompt,"quit\n"));

  return 0;	  
}

char** parsecomm(char* comm)
{
  char** argv = calloc(100, 512*sizeof(char));
  char* token;

  
  /* for(i = 0; i < (int)strlen(comm); ++i) */
  token = strtok(comm, " ");
  do
  {
    printf("%s\n", token);
  }  while ((token = strtok(NULL, " ")) != NULL);

  return argv;
}
