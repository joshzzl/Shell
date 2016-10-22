/*
 * library file for lab2 exercise
 * Author: ZHANG Zhili
 * UID: 3035141243
 */


#include <sys/sysinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define TIMEX "timeX"
#define EXIT "exit"
#define BACKGD "&"
#define SPACE " "
#define PIPE "|"
#define BKG_CHAR '&'

#define STDIN 0
#define STDOUT 1

struct foreground{
  int num;
  int* pids;
};
typedef struct foreground foreground;

typedef int bool;
#define true 1
#define false 0

void sigchld_handler(int signum, siginfo_t *sig, void* context);

void sigusr_handler(int signum);

void sigint_handler(int signum);

int split(char* line, char*** commands, const char* delim);

int getComNum(char* line, char delim);

void usage();

void getInput(char** deref_line, char* buf);

void freeP(int* pid, int num, int* pfd[]);

void freeFg();

int addToFg(int pid);

void fgTerm(int pid);
