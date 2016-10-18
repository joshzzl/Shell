/*
 * library file for lab2 exercise
 * Author: ZHANG Zhili
 */


 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TITLE "PID       CMD       UTIME     STIME     \n"
#define FORMAT_STR "%-10d%-10s%-5lfs    %-5lfs    \n"
#define EXIT "exit"
#define CHILD "&"
#define ERROR_MSG "Executing: %s\nError Message: \n\t%s\n"

typedef int bool;
#define true 1
#define false 0

struct Children{
  int num;
  int* pid;
};

typedef struct Children Children;

int execute(char* argv[]){
    fprintf(stdout, "Current process: %d\n", (int)getpid());
    if(execvp(argv[0], argv)==-1){
      fprintf(stderr, ERROR_MSG, argv[0], strerror(errno));
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void sigchld_handler(int signum, siginfo_t *sig, void* context);
//void sigint_handler(int signum, siginfo_t *sig, void* context);
