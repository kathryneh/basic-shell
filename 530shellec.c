//COMP 530 Assignment 2
//Author: Katie Hawthorne
//Extra credit attempted: part 1 and part 2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>

/* This function uses getenv and strtok to iterate through the 
files in the path, and returns a char* of the path of the file if it exists
according to stat. This char* is then passed to execv.*/
char* checkIfFileExists(char* cmdName){
  //just add a slash to the command since it's a file...
  char commandName[40];
  strcpy(commandName, "/");
  strcat(commandName, cmdName);
  struct stat buffer; // check status
  char* cmdPath = getenv("PATH");
  char* paths[100];
  char* temp;
  int i = 0;
  //the first call to strtok needs to be outside of the while loop
  paths[i] = strtok(cmdPath, ":");
  temp = malloc(strlen(paths[i]) + strlen(commandName)+ 1);
  strcpy(temp, paths[i]);
  strcat(temp, commandName);
  if (stat(temp, &buffer) == 0 ){
      return temp;
  }
  while(1 == 1){ //there's probably a more C-like way of declaring this. But, it works. 
    free(temp);
    i++;
    paths[i] = strtok(NULL, ":");
    if (paths[i] == NULL){
      //it got to the last token; it has to fail here because if it tries to call
      //malloc on null, things break.
      puts("That file cannot be found in any path. Please enter a different command.");
      exit(0);
    }
    temp = malloc(strlen(paths[i]) + strlen(commandName)+ 1);
    strcpy(temp, paths[i]);
    strcat(temp, commandName);
    if (stat(temp, &buffer) == 0 ){
      //a path with this file name is found. Use it. 
      return temp;
    }
  }
  free(temp);
  return "failure";
}
/*this function separates input values into tokens, calls 
checkIfFileExists with the command, and then calls execv*/
char* processLine(char* passedString){
  //if someone's putting more than 100 arguments in, something's wrong.
  //however, need to fool-proof this?
  char *argv[100];
  int i = 0;
  //need to populate the first arg
  argv[i] = strtok(passedString, " \n");
  i++;
  argv[i] = strtok(NULL, " \n");
  while(argv[i]!=NULL){
    i++;
    argv[i] = strtok(NULL, " \n");
  }

  char* sIdentifier;
  pid_t pID = fork();
  pid_t waitPID;
  int status = 0;
  char* tempPath;

  //child fork
  if (pID == 0){
    if (strcmp(argv[0], "cd") == 0){
      puts("Execvp (the command being used) is not allowed to modify the host process.");
      puts("Please try a different command, as cd will not work here");
      exit(0);
    }
    char* tempCmd = malloc(strlen(argv[0])+1);
    strcpy(tempCmd, argv[0]);

    if ((tempPath = checkIfFileExists(tempCmd)) != "failure"){
      //the file exists. Continue onward. 
    }
    else{
      puts("That file cannot be found. Please enter a different command");
      free(tempCmd);
      exit(0);
    }
    execv(tempPath, argv);
    puts("Something went wrong: unknown command");
    free(tempCmd);
    exit(0);
    puts("Not a valid command");
  }
  //failure to fork
  else if (pID < 0){
    exit(1);
    //throw exception
  }

  //parent fork
  else{
    //wait for child to complete.
    waitPID = waitpid(pID, 0, 0);
  }
}
/*this function reads the input line*/
char* readLine(void){
  int bytesRead;
  //do I have to check here if the input is too long?
  size_t numBytes = 1024;
  char *stringIn;

  puts (">>");//a random prompt
  //the maximum number of characters is 1024 for a file name
  //so, I'm allotting for that. 
  stringIn = (char *) malloc (numBytes + 1);
  bytesRead = getline (&stringIn, &numBytes, stdin);
  if (strlen(stringIn) > 1024){
  puts("Invalid input length");
  return NULL;  
  }
  if (bytesRead == -1){
    //failure
      return NULL;
  }
  else{
      return stringIn;
  }
}

int main(int argc, const char * argv[]){
  char* lineIn;
  while((lineIn = readLine()) != NULL){
    processLine(lineIn);
  }
}

