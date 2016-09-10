#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

FILE* CheckFile(const char*);
int ExecBatch(FILE*);
int ExecInter(char**);
char** ParseComm(char*);

int main(int argc, char* argv[])
{
  /* pid_t ch; */
  char line[512];
  
  char** args;
  FILE* batchFile;
  
  /* handle command line arguments */
  if (argc > 1)
  {
    batchFile = CheckFile(argv[1]);
    if(batchFile)
      ExecBatch(batchFile);
    else
      printf("Not Allowed\n");
  }

  /* main shell loop */
  do {
    printf("prompt> ");
    fgets(line, 512, stdin);

    args = ParseComm(line);    /* args gets the dynamically allocated array of 
				  strings returned by ParseComm */
    printf("%s\n", line);
    if(strcmp(line,"quit"))
    {
      printf("%s\n", args[0]);
      batchFile = CheckFile(args[0]);
      if(batchFile)
	ExecBatch(batchFile);
      else
	ExecInter(args);
    }
  } while(strcmp(line,"quit"));

  free(args);
  return 0;	  
}

/* Check to see if program name is batch file 
   (if file exists) */
FILE* CheckFile(const char* fName)
{
  FILE* batchFile = fopen(fName, "r");
  return batchFile;
}

/* Execute commands in a batch file */
int ExecBatch(FILE* bFile)
{
  char line[512];
  char** args;

  while (fgets(line, 512, bFile))
  {
    printf("%s", line);
    args = ParseComm(line);
    ExecInter(args);
  }  
  return 0;
}
  
/* Execute a command in interactive */
int ExecInter(char** args)
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
  }

  return 0;
}

/* Parse the commands that are given  */
char** ParseComm(char* comm)
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
  
  /* if (!strcmp(args[i],"\n")) */
  /*   printf(); */
  
  return args;
}


