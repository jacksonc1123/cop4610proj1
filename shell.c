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
  char** args;
  
  do {
    printf("prompt> ");
    fgets(prompt, 512, stdin);
    args = parsecomm(prompt);
    execute(args);
  } while(strcmp(prompt,"quit"));

  return 0;	  
}

char** parsecomm(char* comm)
{
  char** args = calloc(100, sizeof(char*));
  char* token;
  int i;

  /* Strip newline character */
  i = strlen(comm);
  comm[i-1] = '\0';

  /* Tokenize string into arguments */
  token = strtok(comm, " ");
  i = 0;
  do
  {
    args[i] = calloc(512, sizeof(char));
    strcpy(args[i],token);
    ++i; 
  } while ((token = strtok(NULL, " ")) != NULL);
  args[i] = '\0';
  
  return args;
}

int execute(char** args)
{
  pid_t pid;
  int status;
  
  pid = fork();
  if (pid == 0) /* child */
  {
    execvp(args[0], args);
    fprintf(stderr, "Child failed to execute");
    exit(1);
  }
  else /* parent */
  {
    wait(&status);
    printf("args[0] exited with status: %d\n", status );
  }

  return 0;
}
