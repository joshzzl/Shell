/* Lab 3 - Pipe
*  lab3-shell.c
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define STDOUT 1
#define STDIN 0

int main() {
	char *cmd1[3] = {(char *)"cat", (char *)"demo-txt2.txt", (char *)NULL};
	char *cmd2[3] = {(char *)"grep", (char *)"process", (char *)NULL};
	char *cmd3[3] = {(char *)"wc", (char *)"-l", (char *)NULL};
	
	int pfd1[2], pfd2[2], i;
	
	pipe(pfd1);
	pipe(pfd2);
	
	if (fork() == 0) { //first child
		printf("Execute \"cat demo-txt2.txt\"\n");
		close(pfd2[0]); //close pipe2
		close(pfd2[1]);
		close(pfd1[0]); //close pipe1 read end
		dup2(pfd1[1], STDOUT);  //set pipe1 write end to stdout
		if (execvp(cmd1[0], cmd1) == -1) {
			printf("execvp: error no = %s\n", strerror(errno));
			exit(-1);
		}
	}

	if (fork() == 0) { //second child
		printf("Execute \"grep process\"\n");
		close(pfd1[1]); //close pipe1 write end
		close(pfd2[0]); //close pipe2 read end
		dup2(pfd1[0], STDIN);  //set pipe1 read end to stdin
		dup2(pfd2[1], STDOUT);  //set pipe2 write end to stdout
		if (execvp(cmd2[0], cmd2) == -1) {
			printf("execvp: error no = %s\n", strerror(errno));
			exit(-1);
		}
	}

	if (fork() == 0) { //third child
		printf("Execute \"wc -l\"\n");
		close(pfd1[0]); //close pipe1
		close(pfd1[1]);
		close(pfd2[1]); //close pipe2 write end
		dup2(pfd2[0], STDIN);  //set pipe2 read end to stdin
		if (execvp(cmd3[0], cmd3) == -1) {
			printf("execvp: error no = %s\n", strerror(errno));
			exit(-1);
		}
	}
	
	//parent process
	close(pfd1[0]); //close pipe1
	close(pfd1[1]);	
	close(pfd2[0]); //close pipe2
	close(pfd2[1]);	
	
	for (i=0; i<3; i++)
		wait(NULL);
	
	printf("Program terminated\n");
	return 0;
}
