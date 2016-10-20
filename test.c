/*
	1. get Line
	2. parse into commands with |
	3. check if more than 5 commands

	FOR loop for each command{
		In the first command
			1. check if exit & error handling{
				if so, quit directly and free all pointers
				if error, break
			}
			2. check if timeX & error handling{
				if timeX, then remove "timeX" from the command array
			}

		for all commands except the last one{
			check if any argument itself is "&"

		}
		In the last command
			1. check if "&" exist.
		
		Regular check: if this command has nothing{
			if so, error flag and break directly
		}

	}

	if(error)





*/



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
  int num_Com=0;
  
  // should never be terminated if fgets is working well
  while(line != NULL){

    bool backgd = false;
    
    //change the newline char to nul char
    char *newline = strchr(line, '\n');
    *newline = '\0';
    
    char** commands = NULL;
    num_Com = split(buf, &commands, (char*)"|");
    if(num_Com == -1){
      fprintf(stderr, "Error in splitting\n");
      return EXIT_FAILURE;
    }

    if(num_Com > 5){
      fprintf(stdout, "More than 5 commands were entered!\n");
      free(commands);
      line = fgets(buf, BUFSIZ, stdin);
      
      while(line == NULL && errno != 0){
        errno = 0;
        line = fgets(buf, BUFSIZ, stdin);
      }
      continue;
      
    }

    bool error = false;

    int i;
    for(i=0; i<num_Com; i++){
      //fprintf(stdout, "%s___\n", commands[i]);
      char** args = NULL;
      int length = split(commands[i], &args, (char*)" ");
      
      if(length == -1){
        fprintf(stderr, "Error when splitting %d-th command, exiting the program\n", i);
        error = true;
        break;
      }
    
      if(length == 0){
        fprintf(stderr, "Incomplete | sequence.\n");
        error = true;
        break;
      }
      
      if(i==0 && strcmp(args[0], EXIT)==0 ){
        if(length != 1 || num_Com != 1){
          fprintf(stderr, "\"exit\" with other arguments!!!\n");
          error = true;
          free(args);
          break;
        }
        else{
          fprintf(stdout, "myshell: terminated\n");
          return EXIT_SUCCESS;
        }
      }
      
      int j;
      bool error_and = false;
      for(j=0; j<length-1; j++){
        if(strcmp(args[0], BACKGD) == 0){
          fprintf(stderr, "\'&\' should not appear in the middle of the command line\n");
          error_and = true;
          free(args);
          break;
        }
      }
      if(error_and){
        error = true;
        break;
      }

      char** p = realloc(args, sizeof(char*)*(length+1));
      if(p==NULL){
        fprintf(stderr, "Error in realloc\n");
        error = true;
        break;
      }
      args = p;

      args[length] = (char*)NULL;
      
      char* last = args[length-1];
      if(strcmp(last, BACKGD)==0){
        
        if(i != num_Com-1){
          fprintf(stderr, "\'&\' should not appear in the middle of the command line\n");
          error = true;
          free(args);
          break;
        }
        backgd = true;
        args[length-1] = (char*)NULL;
      }else if(last[strlen(last)-1] == '&'){
        
        if(i != num_Com-1){
          fprintf(stderr, "\'&\' should not appear in the middle of the command line\n");
          error = true;
          free(args);
          break;
        }
        backgd = true;
        last[strlen(last)-1] = '\0';
      }

    }

    if(backgd){
      fprintf(stdout, "background detected\n");
    }

    if(error){
      free(commands);
          
      line = fgets(buf, BUFSIZ, stdin);
      while(line == NULL && errno != 0){
        errno = 0;
        line = fgets(buf, BUFSIZ, stdin);
      }
      continue;
    }
    
      
    /*  
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
    */

    free(commands);
    
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
      fprintf(stderr, "Error in realloc\n");
      return -1;
    }
    args = p;
    args[length-1] = tok;
    tok = strtok_r(NULL, delim, &saveptr);   
  }
  *commands = args;
  return length;
}
