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
				if timeX, then increment args pointer by 1
			}

		for all commands except the last one{
			check if any argument itself is "&"
				if so, error and break

		}


		In the last command{
			1.for every argument except the last one check if it's &
				if so, error and break and break
			2. for the last argument, check if it's "&" or its last digit is '&'
				if so, flag the backgd
		}
		
		Regular check: if this command has nothing{
			if so, error flag and break directly
		}

	}

	if(error){
		print error message
		continue to next loop
	}







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

    
    //change the newline char to nul char
    char *newline = strchr(line, '\n');
    *newline = '\0';
    
    char** commands = NULL;
    num_Com = split(buf, &commands, (char*)"|");
    if(num_Com == -1){
      fprintf(stderr, "Error in splitting\n");
      return EXIT_FAILURE;
    }

    if(num_Com > 5 || num_Com==0 ){
      if(num_Com >5)
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
    bool backgd = false;
    bool timeX = false;
    char*** series=NULL;
    char*** temp = realloc(series, sizeof(char**)*num_Com);
    if(temp==NULL){
      fprintf(stderr, "Error in alloc space for series\n");
      free(commands);
      line = fgets(buf, BUFSIZ, stdin);
      
      while(line == NULL && errno != 0){
        errno = 0;
        line = fgets(buf, BUFSIZ, stdin);
      }
      continue;
    }
    series = temp;

    //Error checking and preparing the arguments for exec()
    int i;
    for(i=0; i<num_Com; i++){
      //fprintf(stdout, "%s___\n", commands[i]);
      char** args = NULL;
      int length = split(commands[i], &args, (char*)" ");
      
      series[i] = args;
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
      
      if(i==0 && strcmp(args[0], TIMEX)==0 ){
        if(length == 1){
          fprintf(stderr, "\"timeX\" cannot be a standalone command");
          error = true;
          free(args);
          break;
        }
        else{
          timeX = true;
        }
      }
      
      bool error_and = false;
      if(i != num_Com -1){   
        int j;
        for(j=0; j<length; j++){
          if(strcmp(args[j], BACKGD) == 0){
            fprintf(stderr, "\'&\' should not appear in the middle of the command line\n");
            error_and = true;
            free(args);
            break;
          }
        }
      }else{
        int j;
        for(j=0; j<length-1; j++){
          if(strcmp(args[j], BACKGD) == 0){
            fprintf(stderr, "\'&\' should not appear in the middle of the command line\n");
            error_and = true;
            free(args);
            break;
          }
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
      
      if(i==num_Com-1){
        char* last = args[length-1];
        if(strcmp(last, BACKGD)==0){
          backgd = true;
          args[length-1] = (char*)NULL;
        }else if(last[strlen(last)-1] == '&'){
          backgd = true;
          last[strlen(last)-1] = '\0';
        }
      }

      series[i] = args;
    }

    if(backgd){
      fprintf(stdout, "background detected\n");
    }

    if(timeX){
      fprintf(stdout, "timeX detected\n");
    }
    if(timeX && backgd){
      fprintf(stderr, "Error dectected: timeX can be used on foreground only\n");
      error = true;
    }
    
    if(error){
      for(i=0; i<num_Com; i++){
        if(series[i] != NULL)
          free(series[i]);
      }
      if(commands != NULL)
        free(commands);
      if(series != NULL)
        free(series);
          
      line = fgets(buf, BUFSIZ, stdin);
      while(line == NULL && errno != 0){
        errno = 0;
        line = fgets(buf, BUFSIZ, stdin);
      }
      continue;
    }
    
    
    for(i=0; i<num_Com; i++){
      int j=((timeX && i==0)? 1 : 0);
      while(series[i][j]!= NULL){
        fprintf(stdout, "%s ", series[i][j++]);
      }
      fprintf(stdout, "\n");
    }
    
    
    //fprintf(stdout, "still okay at 253\n");
    
    bool error_prep = false;
    int pip_num = num_Com -1;
    
    int* pid_com=NULL; // the array storing all pids for child processes
    int* pid_temp = realloc(pid_com, sizeof(int)*num_Com);
    if(pid_temp == NULL){
      error_prep = true;
      fprintf(stderr, "error in alloc pids\n");
    }
    
    int** pfd=NULL; // the pointer points to the pipes
    //fprintf(stdout, "Pipe number: %d\n", pip_num);
    if(pip_num>0){
      int** pfd_temp = realloc(pfd, sizeof(int*)*pip_num);
      if(pfd_temp == NULL){
        error_prep = true;
        fprintf(stderr, "error in alloc pfds\n");
      }else{
        pfd = pfd_temp;
        int i;
        for(i=0; i<pip_num; i++){
          pfd[i]=NULL;
          int* pip_temp = malloc(sizeof(int)*2);
          if(pip_temp != NULL){
            pfd[i]=pip_temp;
          }else{
            error_prep = true;
          }
        }
      }
    }

    if(error_prep){
      fprintf(stderr, "Error_prep\n");
      if(pid_com != NULL)
        free(pid_com);
      if(pip_num>0 && pfd != NULL){
        int i;
        for(i=0; i<pip_num; i++){
          if(pfd[i] != NULL)
            free(pfd[i]);
        }
        free(pfd);
      }
      
      for(i=0; i<num_Com; i++){
        if(series[i] != NULL)
          free(series[i]);
      }
      free(commands);
      free(series);
          
      line = fgets(buf, BUFSIZ, stdin);
      while(line == NULL && errno != 0){
        errno = 0;
        line = fgets(buf, BUFSIZ, stdin);
      }
      continue;
      
    }

    for(i=0; i<pip_num; i++){
      pipe(pfd[i]);
    }
    
    fprintf(stdout, "still okay at 317\n");
    int pid_test; 
    bool error_exe=false;
    for(i=0; i<num_Com; i++){
      int pid = fork();
      pid_test = pid;
      //in child process
      if(pid == 0){

        signal(SIGINT, SIG_DFL);
        
        //close the corresponding pipes
        
        if(i==0){
          
          int j;
          for(j=0; j<pip_num; j++){
            if(j==0){
              close(pfd[j][0]);
            }else{
              close(pfd[j][0]);
              close(pfd[j][1]);
            }
          }
          if(pip_num>0)
            dup2(pfd[0][1], STDOUT);
        
        }else if(i==num_Com-1){
          
          int j;
          for(j=0; j<pip_num; j++){
            if(j==pip_num-1){
              close(pfd[j][1]);
            }else{
              close(pfd[j][0]);
              close(pfd[j][1]);
            }
          }
          if(pip_num>0)
            dup2(pfd[pip_num-1][1], STDIN);
         
        }else{
         
          int j;
          for(j=0; j<pip_num; j++){
            if(j==i-1){
              close(pfd[j][1]);
            }else if(j==i){
              close(pfd[j][0]);
            }else{
              close(pfd[j][0]);
              close(pfd[j][1]);
            }
          }
          dup2(pfd[i-1][0], STDIN);
          dup2(pfd[i][1], STDOUT);
         
        }
        
        fprintf(stdout, "Still okay at 376\n");
        if(execvp(series[i][0], series[i])==-1){
          fprintf(stderr, "execvp: error no = %s\n", strerror(errno));
          exit(-1);
        }
      
      }else if(pid < 0){
        error_exe=true;
        break;
      }else{
        fprintf(stdout, "Still okay at 386\n");
        
        /*if(!backgd){
          pid_com[i] = pid; 
        }*/
        fprintf(stdout, "Still okay at 395\n");
      }
    }

    int j;
    for(j=0; j<pip_num; j++){
      close(pfd[i][0]);
      close(pfd[i][1]);
    }
    

    if(error_exe){
      fprintf(stdout, "error_exe!!!\n");

      if(pid_com != NULL)
        free(pid_com);
      if(pfd != NULL){
        int i;
        for(i=0; i<pip_num; i++){
          if(pfd[i] != NULL)
            free(pfd[i]);
        }
        free(pfd);
      }
      
      for(i=0; i<num_Com; i++){
        if(series[i] != NULL)
          free(series[i]);
      }
      free(commands);
      free(series);
          
      line = fgets(buf, BUFSIZ, stdin);
      while(line == NULL && errno != 0){
        errno = 0;
        line = fgets(buf, BUFSIZ, stdin);
      }
      continue;
      
    }
    
    fprintf(stdout, "still okay at 423\n");
    if(!backgd){
      int i;
      for(i=0; i<num_Com; i++){
        int status;
        waitpid(pid_test, &status, 0);

        //if(pid_com[i]>0)
          //waitpid(pid_com[i], &status, 0);
      }
    }

    if(pid_com != NULL)
      free(pid_com);
    if(pfd != NULL){
      int i;
      for(i=0; i<pip_num; i++){
        if(pfd[i] != NULL)
          free(pfd[i]);
      }
      free(pfd);
    }
    for(i=0; i<num_Com; i++){
      if(series[i] != NULL)
        free(series[i]);
    }
    free(commands);
    free(series);
    
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
