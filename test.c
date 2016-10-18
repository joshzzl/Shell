#include "lab3.h"

int parent;

int main(){
  //retrieve the parent pid
  parent = (int)getpid();
  
  struct sigaction sc;
  sigaction(SIGCHLD, NULL, &sc);
  sc.sa_flags = SA_SIGINFO;
  sc.sa_sigaction = sigchld_handler;
  sigaction(SIGCHLD, &sc, NULL);
  
  signal(SIGINT, SIG_IGN);
  
  //Children children = {.num = 0, .pid = NULL};
  
  char buf[BUFSIZ];
  //input line
  char* line = fgets(buf, BUFSIZ, stdin);
  int length;
  
  // should never be terminated if fgets is working well
  while(line != NULL){
    
    //change the newline char to nul char
    char *newline = strchr(line, '\n');
    *newline = '\0';
    
    length = 0;  //length of the argument array
    
    char **args=NULL; 
    char *tok = strtok(buf, " ");
    
    while(tok != NULL){ // if the user entered something
       
      length++;

      // alloc one more place to store the NULL
      char** p = realloc(args, sizeof(char*) * (length+1));
      if(p==NULL){
        fprintf(stderr, "Error in realloc\n");
        return EXIT_FAILURE;
      }
      args = p;
      args[length-1] = tok;
      tok = strtok(NULL, " ");   
    }

    if(length == 0){
      line = fgets(buf, BUFSIZ, stdin);
      while(line == NULL && errno != 0){
        errno = 0;
        line = fgets(buf, BUFSIZ, stdin);
      }
      continue;
    }

    //if the user entered something and the first token is "exit"
    if(strcmp(args[0], EXIT) == 0){
      if(length == 1){
        fprintf(stdout, "Program terminated\n");
        return EXIT_SUCCESS;
      }else{
        fprintf(stderr, "\"exit\" with other arguments!\n");
        free(args);
        
        line = fgets(buf, BUFSIZ, stdin);
        while(line == NULL && errno != 0){
          errno = 0;
          line = fgets(buf, BUFSIZ, stdin);
        }
        continue;
      }
    }
    
    args[length] = (char*)NULL;
      
    char* last = args[length-1];
    bool backgd = false; 
    if(strcmp(last, CHILD)==0){
      //creating child process to handle
      backgd = true;
      args[length-1] = (char*)NULL;
    }else if(last[strlen(last)-1] == '&'){
      backgd = true;
      last[strlen(last)-1] = '\0';
    }
      
      
      
    pid_t pid = fork();

    if(pid<0){
      fprintf(stderr, "\nError in creating new process\n");
      return EXIT_FAILURE;
    }else if(pid == 0){
      if(!backgd)
        signal(SIGINT, SIG_DFL);
      return execute(args);
    }else{
        
      if(!backgd){
        pid_t child_pid;
        int status;
        child_pid = waitpid(pid, &status, 0);
    
        fprintf(stdout, "Child process (%d) exited, with status %d\n\n", (int)child_pid, WEXITSTATUS(status));
      }
    }
 
    free(args);
    
    line = fgets(buf, BUFSIZ, stdin);
    while(line == NULL && errno != 0){
      errno = 0;
      line = fgets(buf, BUFSIZ, stdin);
    }
  }

  fprintf(stderr, "Something wrong in fgets()\n");
  //should never reach this point.
  return EXIT_FAILURE;
}


void sigchld_handler(int signum, siginfo_t *sig, void* context){
  pid_t child_pid;
  int status;
  child_pid = waitpid(-1, &status, WNOHANG);
  while(child_pid > 0){
    fprintf(stdout, "Background child process (%d) exited, with status %d\n\n", (int)child_pid, WEXITSTATUS(status));
    child_pid = waitpid(-1, &status, WNOHANG);
  }
}



