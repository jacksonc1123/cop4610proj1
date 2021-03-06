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
void ParseComm(char**,char*,char*);
int Empty(const char*);

const int BUFF = 515;

int main(int argc, char* argv[]){
  char* line;
  FILE* batchFile;
  char** args = NULL; 
  int execStatus = 0;
  int i, eCode = 0;

  line = calloc(BUFF,sizeof(char));
  /* handle command line arguments */
  if(argc > 1) {
    batchFile = CheckFile(argv[1]);
    if(!batchFile) {
      fprintf(stderr, "%s: File not found\n", argv[1]);
      exit(1);
    }
    ExecBatch(batchFile);
    return 0;
  }
  else if(argc > 2) {
    fprintf(stderr, "Too many arguments\n");
    exit(1);
  }

  args = (char**)calloc(100, sizeof(char*));
  /* main shell loop */
  do {
    printf("prompt> ");
    fgets(line, BUFF, stdin);
    if(feof(stdin))
      eCode = 1;
    /* handle error with input */
    if(line == NULL)
      fprintf(stderr, "There was an error processing your request\n");
    else if(strlen(line) > 512)
      fprintf(stderr, "Command too long\n");
    else if(!Empty(line)) {
      /* args gets the dynamically allocated array of strings returned by 
	 ParseComm */
      ParseComm(args,line,";"); 
      if(args[0] != NULL) { /* only run the following if args isn't empty */
	execStatus = ExecInter(args);
	/* handle quit signals not caught by parsing */
	if(execStatus == 1)
	  break;
       
	/* free up dynamically allocated memory */
	if(args != NULL) {
	  BigFree(args);
	}
      }
    }
  } while(strcmp(line,"quit") && (eCode != 1));

  if(args != NULL) {
    BigFree(args);
    free(args);
  }

  return 0;	  
}

/* Check to see if program name is batch file and open
   (if file exists) */
FILE* CheckFile(const char* fName) {
  FILE* batchFile = fopen(fName, "r");
  return batchFile;
}

/* Execute commands in a batch file */
int ExecBatch(FILE* bFile) {
  char line[BUFF];
  char** args;

  while(fgets(line, BUFF, bFile)) {
    /* handle error with input */
    if(!line)
      fprintf(stderr, "There was an error processing your request\n");
    else if(!strcmp(line,"quit"))
      return 1;
    ParseComm(args, line,";");

    ExecInter(args);
  }  
  return 0;
}
  
/* Execute a command or list of commands.
   If quit is one of the arguments, the function skips the fork/exec loop,
   and sets a return variable to 1 so the main loop can properly exit. */
int ExecInter(char** args) {
  pid_t pid, c_pid;
  int status, i, r_code;
  char** tList = NULL;

  tList = (char**)calloc(100, sizeof(char*));
  i = r_code = 0;
  /* fork and exec for every argument */
  while(args[i] != NULL) {
    printf("%s\n",args[i]);
    if (!strstr(args[i],"quit") ||
	(strstr(args[i],"quit") && (strlen(args[i]) > 5))) { 
      /* only fork if the not the quit command */
      pid = fork();
      ParseComm(tList, args[i]," ");
      if(pid == 0) { /* child */
	execvp(tList[0], tList);
	fprintf(stderr, "%s: Command not found\n", args[i]);
	exit(1);
      }
      ++i;
    }
    else if(strstr(args[i],"quit") && (strlen(args[i]) < 6)) {
      r_code = 1;
      ++i;
    }
  }

  i = 0;
  /* wait for every argument */
  while(args[i] != NULL) {
    c_pid = wait(&status);
    ++i;
  }

  BigFree(tList);
  return r_code;
}

/* Parse the command lines that are given  */
void ParseComm(char** args, char* comm, char* delim) {
  char* token;
  int i, j, k;

  /* Strip newline character */
  i = 0;
  while(comm[i] != '\0') {
    if(comm[i] == '\n')
      comm[i] = '\0';
    ++i;
  }
 
  /* Tokenize string into arguments */
  token = strtok(comm, delim);
  /* if token is empty here, then it was an empty command */
  if(!token) {
    BigFree(args);
    return;
  }

  i = 0;
  do {
    if (Empty(token)) {
      token = strtok(NULL, delim);
    }
    args[i] = (char*)calloc(strlen(token)+1, sizeof(char));
    strcpy(args[i],token);
    ++i;
  } while((token = strtok(NULL, delim)) != NULL);
}

/* Check if string is only whitespace */
int Empty(const char* str) {
  int i = 0;
  while(str[i] != '\0') {
    if(!isspace((int)str[i]))
      return 0;
    ++i;
  }

  return 1;
}

/* free the dynamically allocated structures */
void BigFree(char** args) {
  int i = 0;
  while (args[i] != NULL) {
    free(args[i]);
    args[i] = NULL;
    ++i;
  }
}
