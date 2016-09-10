#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int ArraySize(char**);
FILE* CheckFile(const char*);
int Count(char*, char**);
int ExecBatch(FILE*);
int ExecInter(char**);
char** ParseComm(char*);
char*** ParseLine(char**);

int main(int argc, char* argv[])
{
  /* pid_t ch; */
  char line[512];
  char*** argLists;
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
    argLists = ParseLine(args);
    if(strcmp(line,"quit"))
    {
      /* printf("%s\n", args[0]); */
      batchFile = CheckFile(args[0]);
      if(batchFile)
    	ExecBatch(batchFile);
      else
    	ExecInter(args);
    }
  } while(strcmp(line,"quit"));

  free(args);
  free(argLists);
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
    /* allocate enough space for the size of the token */
    args[i] = calloc(strlen(token+1), sizeof(char));
    strcpy(args[i],token);

    ++i; 
  } while ((token = strtok(NULL, " ")) != NULL);
  

  /* if (!strcmp(args[i],"\n")) */
  /*   printf(); */
  
  return args;
}

/* split the list of arguments into another list with semicolons as the
   delimiter*/
char*** ParseLine(char** args)
{
  char*** commands;
  int size, count;
  int i, j, k;

  size = ArraySize(args);
  count = Count(";", args);
  commands = calloc(size, sizeof(char**));

  i = 0;
  j = 0;
  k = 0;

  printf("Number of commands: %d\n", count);
  /* allocate space for each command */


  while(args[i] != NULL)
  {
    strcpy(commands[j][k],args[i]);
    printf("%s\n",commands[j][k]);
    ++i;
    ++k;
    if (!strcmp(args[i],";"))
    {
      ++j;
      k = 0;
    }
  }

  return commands;
}

int ArraySize(char** args)
{
  int i = 0;
  
  while(args[i] != NULL)
    ++i;

  return i;
}

/* simple helper to count number of specified delimiter in list*/
int Count(char* delim, char** args)
{
  int i = 0;
  int count = 0;

  while(args[i] != NULL)
  {
    printf("%s\n",args[i]);
    if(!strcmp(args[i],delim))
      ++count;
    ++i;
  }
  
  return count+1;
}
