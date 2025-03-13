#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "execline.h"
#include <stdio.h>
#include <sys/wait.h>

//if buitin command
int _if(int argc,char **argv){
	if(lock > 0){
		lock++;
		return 0;
	}

	if(argc< 2){
		fprintf(stderr,"if : no program specified\n");
		return -1;
	}
	//launch the program
	pid_t child = fork();
	if(child < 0){
		perror("fork");
		return -1;
	}
	if(child == 0){
		execvp(argv[1],&argv[1]);
		fprintf(stderr,"%s : %s\n",argv[1],strerror(errno));
		return errno;
	}

	int status;
	waitpid(child,&status,0);
	if(WIFEXITED(status) && (WEXITSTATUS(status) == 0)){
		return 0;
	}
	lock = 1;
	return 0;
}
