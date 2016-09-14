#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

void BigFree(char**);
FILE* CheckFile(const char*);
int ExecBatch(FILE*);
int ExecInter(char**);
char** ParseComm(char*,char*);
int Empty(const char*);

int main(int argc, char* argv[])
{
  char line[512];
  char** args;
  FILE* batchFile;
  int execStatus;
  
  /* handle command line arguments */
  if(argc > 1)
  {
    batchFile = CheckFile(argv[1]);
    if(!batchFile)
    {
      fprintf(stderr, "%s: File not found\n", argv[1]);
      exit(1);
    }
    else
      execStatus = ExecBatch(batchFile);
  }
  else if(argc > 2)
  {
    fprintf(stderr, "Too many arguments\n");
    exit(1);
  }

  if(execStatus == 1)
    return 0;
  /* main shell loop */
  do {
    printf("prompt> ");
    fgets(line, 512, stdin);
    /* handle error with input */
    if(line == NULL)
    {
      fprintf(stderr, "There was an error processing your request.\n");
    }
    else if(!Empty(line))
    {
      /* args gets the dynamically allocated array of strings returned by 
	 ParseComm */
      args = ParseComm(line,";"); 
      if(strcmp(line,"quit"))
      {
	batchFile = CheckFile(args[0]);
	if(batchFile)
	  execStatus = ExecBatch(batchFile);
	else
	  ExecInter(args);
      }
      /* handle quit command in batch file processing */
      if(execStatus == 1)
	break;
      /* free up dynamically allocated memory */
      BigFree(args);
    }
  } while(strcmp(line,"quit"));

  return 0;	  
}

/* Check to see if program name is batch file and open
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

  while(fgets(line, 512, bFile))
  {
    /* handle error with input */
    if(!line)
    {
      fprintf(stderr, "There was an error processing your request.\n");
    }    
    else if(!strcmp(line,"quit"))
      return 1;
    args = ParseComm(line,";");

    ExecInter(args);
  }  
  return 0;
}
  
/* Execute a command in interactive */
int ExecInter(char** args)
{
  pid_t pid, c_pid;
  int status, i;
  char** tList;
  
  i = 0;
  while(args[i] != NULL)
  {
    /* printf("Command: %s\n",args[i]); */
    pid = fork();
    tList = ParseComm(args[i]," ");
    if (pid == 0) /* child */
    {
      execvp(tList[0], tList);
      fprintf(stderr, "%s: Command not found\n", args[i]);
      exit(1);
    } 
    ++i;
  }

  i = 0;
  while(args[i] != NULL)
  {
    c_pid = wait(&status);
    ++i;
  }

  return 0;
}

/* Parse the commands that are given  */
char** ParseComm(char* comm, char* delim)
{
  char** args = (char**)calloc(100, sizeof(char*));
  char* token;
  int i, j, k;

  /* Strip newline character */
  i = 0;
  while(comm[i] != '\0')
  {
    if(comm[i] == '\n')
      comm[i] = '\0';
    ++i;
  }

  /* Tokenize string into arguments */
  token = strtok(comm, delim);
  i = 0;
  do {
    if (Empty(token))
    {
      token = strtok(NULL, delim);
    }

    args[i] = (char*)calloc(strlen(token)+1, sizeof(char));
    strcpy(args[i],token);
    ++i;

  } while((token = strtok(NULL, delim)) != NULL);
  
  /* if (!strcmp(args[i],"\n")) */
  /*   printf(); */
  
  return args;
}

int Empty(const char* str)
{
  int i = 0;
  while(str[i] != '\0')
  {
    if(!isspace((int)str[i]))
      return 0;
    ++i;
  }

  return 1;
}

void BigFree(char** args)
{
  int i = 0;

  while (args[i] != NULL)
  {
    free(args[i]);
    ++i;
  }
  free(args);
}
