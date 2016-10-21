/*
 * Helper methods for shell
 */

#include "shellStrings.h"
#include "lab3.h"

extern int usr_flag;


void sigchld_handler(int signum, siginfo_t *sig, void* context){
  pid_t child_pid;
  int status;
  child_pid = waitpid(-1, &status, WNOHANG);
  while(child_pid > 0){
    fprintf(stdout, COMPLETE_MSG, (int)child_pid);
    child_pid = waitpid(-1, &status, WNOHANG);
  }
}

void sigusr_handler(int signum){
  usr_flag=true;
}


int split(char* line, char*** commands, const char* delim){
  
  int length = 0;  //length of the argument array
    
  char **args=NULL;
  char* saveptr;
  char *tok = strtok_r(line, delim, &saveptr);
    
  while(tok != NULL){ // if the user entered something
       
    length++;

    // alloc one more place to store the NULL
    char** p = realloc(args, sizeof(char*) * (length));
    if(p==NULL){
      fprintf(stderr, ERR_REALLOC);
      return -1;
    }
    args = p;
    args[length-1] = tok;
    tok = strtok_r(NULL, delim, &saveptr);   
  }
  *commands = args;
  return length;
}

int getComNum(char* line, char delim){
  if((*line)=='\0')
    return 0;
  int len = 1;
  char* next = strchr(line, (int)delim);
  while(next != NULL){
    len++;
    line = ++next;
    next = strchr(line, (int)delim);
  }
  return len;
}

void getInput(char** deref_line, char* buf){
  usage();
  char* line = fgets(buf, BUFSIZ, stdin);
      
  while(line == NULL && errno != 0){
    errno = 0;
    usage();
    line = fgets(buf, BUFSIZ, stdin);
  }
  *deref_line = line;
}

void freeP(int* pid, int num, int* pfd[]){
  if(pid != NULL)
        free(pid);
      if(pfd != NULL){
        int i;
        for(i=0; i<num; i++){
          if(pfd[i] != NULL)
            free(pfd[i]);
        }
        free(pfd);
      }
}

void usage(){
  fprintf(stdout, USGMSG);
}