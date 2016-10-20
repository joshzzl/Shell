/*
 * Helper function
 */


#include "lab3.h"

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stdout, "Only 1 argument should be entered\n");
  }
  char *endptr;
  long loop = strtol(argv[1], &endptr, 10);
  //fprintf(stdout, "%ld\n", loop);
  sleep(loop);
  fprintf(stdout, "Finish sleeping.\n");
  return 0;
}
