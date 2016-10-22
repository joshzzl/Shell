/*
 * Author: ZHANG Zhili
 * UID: 3035141243
 */


#include "shellStrings.h"
#include "myshell.h"

bool usr_flag; //used by SIGUSR1 handler
foreground fg;

int main(){

  //SIGCHLD handler
  struct sigaction sc;
  sigaction(SIGCHLD, NULL, &sc);
  sc.sa_flags = SA_SIGINFO;
  sc.sa_sigaction = sigchld_handler;
  sigaction(SIGCHLD, &sc, NULL);
  
  fg.num = 0;
  fg.pids = NULL;
  //Ignore the SIGINT
  struct sigaction si;
  sigaction(SIGINT, NULL, &si);
  si.sa_flags = SA_SIGINFO;
  si.sa_handler = sigint_handler;
  sigaction(SIGINT, &si, NULL);
  
  //signal(SIGINT, sigint_handler);
    
  //input line
  char* line = NULL;
  char buf[BUFSIZ];
  getInput(&line, buf);

  int num_Com=0;
  
  // should never be terminated if getInput is working well
  while(line != NULL){

    usr_flag=false;
    
    //change the newline char to nul char
    char *newline = strchr(line, '\n');
    *newline = '\0';

    //split the entire line into different commands
    char** commands = NULL;
    num_Com = split(buf, &commands, (char*)PIPE);
    //Error in spliting the input line
    if(num_Com == -1){
      fprintf(stderr, ERR_SPLIT);
      if(commands!=NULL)
        free(commands);
      continue;
    }

    if(num_Com > 5 || num_Com==0 ){ //error checking for no command
      if(num_Com >5)
        fprintf(stdout, ERR_5_ARG);
      if(commands != NULL)
        free(commands);
      getInput(&line, buf);
      continue;
    }

    //allocate space for pointers pointing different argument arrays
    bool error = false;
    bool backgd = false;
    bool timeX = false;
    char*** series=NULL;
    char*** temp = realloc(series, sizeof(char**)*num_Com);
    if(temp==NULL){
      fprintf(stderr, ERR_ALLOC_SERIES);
      free(commands);
      getInput(&line, buf);
      continue;
    }
    series = temp;
    

    //Error checking and preparing the arguments for exec()
    int i;
    for(i=0; i<num_Com; i++)
      series[i]=NULL;
    for(i=0; i<num_Com; i++){
      char** args = NULL;
      int length = split(commands[i], &args, (char*)SPACE);
      
      series[i] = args;
      if(length == -1){
        fprintf(stderr, ERR_SPLIT_N, i);
        error = true;
        break;
      }
    
      if(length == 0){
        fprintf(stderr, ERR_INCOM_PIPE);
        error = true;
        break;
      }
      // if "exit" is entered as the first argument in the first command
      if(i==0 && strcmp(args[0], EXIT)==0 ){
        if(length != 1 || num_Com != 1){
          fprintf(stderr, ERR_EXIT_ARG);
          error = true;
          break;
        }
        else{
          free(args);
          free(commands);
          free(series);
          freeFg();
          fprintf(stdout, TERM_MSG);
          return EXIT_SUCCESS;
        }
      }
      // if "timeX" is entered as the first argument in the first command
      if(i==0 && strcmp(args[0], TIMEX)==0 ){
        if(length == 1){
          fprintf(stderr, ERR_TIMEX_ALONE);
          error = true;
          break;
        }
        else{
          timeX = true;
        }
      }
      
      bool error_and = false;
      //get rid of the cases when '&' is wrongly used
      if(i != num_Com -1){   
        int j;
        for(j=0; j<length; j++){
          if(strcmp(args[j], BACKGD) == 0){
            fprintf(stderr, ERR_AND_MIDDLE);
            error_and = true;
            break;
          }
        }
      }else{
        int j;
        for(j=0; j<length-1; j++){
          if(strcmp(args[j], BACKGD) == 0){
            fprintf(stderr, ERR_AND_MIDDLE);
            error_and = true;
            break;
          }
        }
      }
      if(error_and){
        error = true;
        break;
      }
      //allocate one more space for (char*)NULL
      char** p = realloc(args, sizeof(char*)*(length+1));
      if(p==NULL){
        fprintf(stderr, ERR_REALLOC);
        error = true;
        break;
      }
      args = p;

      args[length] = (char*)NULL;
      //for the  last command only, check if the last argument 
      //or its last character is "&"('&')
      if(i==num_Com-1){
        char* last = args[length-1];
        if(strcmp(last, BACKGD)==0){
          backgd = true;
          args[length-1] = (char*)NULL;
        }else if(last[strlen(last)-1] == BKG_CHAR){
          backgd = true;
          last[strlen(last)-1] = '\0';
        }
      }

      series[i] = args;
    }
    /*
    if(backgd){
      fprintf(stdout, "backgd detected\n");
    }*/
    //if timeX and & both entered, error
    if(timeX && backgd){
      fprintf(stderr, ERR_TIMEX_BCG);
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
      getInput(&line, buf);
      continue;
    }
    
    bool error_prep = false;
    //fprintf(stdout, "num_Com %d\n", num_Com);
    int pip_num = num_Com -1;
    
    //prepare the pipes for execution
    //also allocate spaces to store pids of child processes
    int* pid_com=NULL; // the array storing all pids for child processes
    int* pid_temp = malloc(sizeof(int)*num_Com);
    if(pid_temp == NULL){
      error_prep = true;
      fprintf(stderr, ERR_MAL_PID);
    }
    pid_com = pid_temp;

    for(i=0; i<num_Com; i++){
      pid_com[i]=0;
    }
    
    int** pfd=NULL; // the pointer points to the pipes

    if(pip_num>0){
      int** pfd_temp = malloc(sizeof(int*)*pip_num);
      if(pfd_temp == NULL){
        error_prep = true;
        fprintf(stderr, ERR_ALLOC_PFD);
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
      freeP(pid_com, pip_num, pfd);
      
      for(i=0; i<num_Com; i++){
        if(series[i] != NULL)
          free(series[i]);
      }
      free(commands);
      free(series);
      
      getInput(&line, buf);
      continue;
      
    }

    for(i=0; i<pip_num; i++){
      pipe(pfd[i]);
    }
    
    bool error_exe=false;
    for(i=0; i<num_Com; i++){
      
      //install the SIGUSR1 handler
      signal(SIGUSR1, sigusr_handler);
      int pid = fork();
      //in child process
      if(pid == 0){
        //Make the SIGINT handling default
        if(!backgd)
          signal(SIGINT, SIG_DFL);
        else
          signal(SIGINT, SIG_IGN);
        
        //close the corresponding pipes
        if(i==0){//the case of the first command
          
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
        
        }else if(i==num_Com-1){//the case of the last command
          
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
            dup2(pfd[pip_num-1][0], STDIN);
         
        }else{//the case of commands that are between pipes
         
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

        while(!usr_flag);
        
        //define the command for execvp
        //take 'timeX' effect into consideration
        //ex. timeX ls -lat, then the command
        //should be 'ls' instead of 'timeX'
        char* com1= ( i==0 ? ( timeX ? series[i][1] : series[i][0]):series[i][0]);
        char** com2 = series[i];
        if(timeX && i==0)
          com2++;
        
        if(execvp(com1, com2)==-1){
          fprintf(stderr, ERR_EXECVP, strerror(errno));
          exit(-1);
        }
      
      }else if(pid < 0){
        error_exe=true;
        break;
      }else{
        //store the pid into pid_com
        pid_com[i] = pid; 

        if(!backgd){
          if(addToFg(pid) == -1){
            error_exe=false;
            break;
          }
        }
        
        //send the SIGUSR1 in order to control its execution
        kill(pid, SIGUSR1);
      }
    }
    //in parent, close all pipes
    if(pip_num>0){
      int j;
      for(j=0; j<pip_num; j++){
        close(pfd[j][0]);
        close(pfd[j][1]);
      }
    }

    if(error_exe){
      freeP(pid_com, pip_num, pfd);
      
      for(i=0; i<num_Com; i++){
        if(series[i] != NULL)
          free(series[i]);
      }
      free(commands);
      free(series);
          
      getInput(&line, buf);
      continue;
      
    }
    sleep(1);
    //if timeX is applied
    if(timeX){
      int k;
      //for each command
      for(k=0; k<num_Com; k++){
        siginfo_t info;//obtain the siginfo_t
        int result = waitid(P_PID, pid_com[k], &info, WNOWAIT | WEXITED);
        if(result != 0){
          fprintf(stderr, ERR_WAITID, pid_com[k], strerror(errno));
        }

        char str[50];
        sprintf(str, PROC_STAT, (int)pid_com[k]);
        FILE *file = fopen(str, "r");
        if (file == NULL) {
          printf(ERR_OPEN_PROC);
          continue;
        }
        int z;
        unsigned long h, ut, st;
        long dum;
        unsigned long long stime;
        fscanf(file, SCAN_F, &z, str, &str[0], &z, &z, &z, &z, &z,
          (unsigned *)&z, &h, &h, &h, &h, &ut, &st, &dum, &dum, &dum, &dum, &dum, &dum, &stime);
        fclose(file);

        //this is the time that the program starts
        double start = (double)(stime*1.0f/sysconf(_SC_CLK_TCK));

        struct sysinfo sinfo;
        sysinfo(&sinfo);
        //sysinfo.uptime is the time stamp of right now
        //here we find the difference between uptime and start
        //so that we can obtain the run time
        double rtime = (double)sinfo.uptime - start;
        char* prog = (k==0 ? series[k][1] : series[k][0]);
        fprintf(stdout, TITLE);
        fprintf(stdout, FORMAT_STR, (int)pid_com[k], prog, rtime, 
          ut*1.0f/sysconf(_SC_CLK_TCK), st*1.0f/sysconf(_SC_CLK_TCK));
    
        int status;
        waitpid(pid_com[k], &status, 0);
        
        fgTerm(pid_com[k]);
      }      
    }else{
      if(!backgd){
        int i;
        for(i=0; i<num_Com; i++){
          int status;
          if(pid_com[i]>0){
            waitpid(pid_com[i], &status, 0);
            fgTerm(pid_com[i]);
          }
        }
      }
    }
    //free the space alloc in introducing pipes
    freeP(pid_com, pip_num, pfd);
    //free other allocated spaces
    for(i=0; i<num_Com; i++){
      if(series[i] != NULL)
        free(series[i]);
    }
    free(commands);
    free(series);
    getInput(&line, buf);
  }

  fprintf(stderr, ERR_FGETS);
  //should never reach this point.
  return EXIT_FAILURE;
}