/*
 * Author: ZHANG Zhili
 * UID: 3035141243
 */

/*
 * Helper methods for shell
 */
#include "shellStrings.h"
#include "myshell.h"

extern int usr_flag;
extern foreground fg;

//SIGCHLD handler
void sigchld_handler(int signum, siginfo_t *sig, void* context){
  int id = (int)sig->si_pid;
  int i;
  bool fgProcess=false;
  for(i=0; i<fg.num; i++){
    if(fg.pids[i]==id)
      fgProcess=true;
  }
  if(fgProcess)
    return;
  pid_t child_pid;
  int status;
  child_pid = waitpid(-1, &status, WNOHANG);
  while(child_pid > 0){
    fprintf(stdout, COMPLETE_MSG, (int)child_pid);
    child_pid = waitpid(-1, &status, WNOHANG);
  }
}

//modify the fg.pids when a foreground child process is terminating
void fgTerm(int pid){
  int i;
  for(i=0; i<fg.num; i++){
    if(fg.pids[i]==pid)
      fg.pids[i]=-1;
  }
}

//SIGUSR handler
void sigusr_handler(int signum){
  usr_flag=true;
}

//SIGINT handler
void sigint_handler(int signum){
  fprintf(stdout, "\n");
  //usage();
}

//split the entire line into commands with delim as delimiter
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

//a method for debugging, used to count the number of '|'
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

//get input from stdin and store it in the
//previously allocated spaces
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

//free the space allocated when introducing pipes
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

//free the space allocated in struct foreground
void freeFg(){
  if(fg.pids!=NULL){
    free(fg.pids);
  }
}

//add a pid to fg.pids
int addToFg(int pid){
  fg.num++;
  int* temp = realloc(fg.pids, sizeof(int)*fg.num);
  if(temp == NULL){
    return -1;
  }
  fg.pids = temp;
  fg.pids[fg.num-1]=pid;
  return 0;
}

//print the shell cursor
void usage(){
  fprintf(stdout, USGMSG);
}
