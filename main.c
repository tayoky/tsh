#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "tsh.h"

int main(int argc,char **argv){
	init(argc,argv);
	if(argc < 2){
		return shell_mode();
	} else {
		if(!strcmp(argv[1],"--version")){
			return interpret("version");
		}
		if(!strcmp(argv[1],"-c")){
			//execute one single command
			if(argc < 3){
				printf("tsh : -c option require one argument\n");
				return -1;
			}
			return interpret(strdup(argv[2]));
		}
		return script_mode(argv[1]);
	}
}

int script_mode(const char *path){
	FILE *file = fopen(path,"r");
	if(file == NULL){
		printf("%s : %s\n",path,strerror(errno));
		exit(-1);
	}
	interpret(file);
	fclose(file);
	return 0;
}

int shell_mode(void){
	for(;;){
		//show the prompt
		char cwd[256];
		getcwd(cwd,255);
	
		char *home = getenv("HOME");

		//default home
		if(!home){
			home = "/";
		}

		//if in home directory show ~
		if(!strncmp(home,cwd,strlen(home))){
			printf("~%s $ ",&cwd[strlen(home)]);
		} else {
			printf("%s $ ",cwd);
		}


		interpret(stdin);

	}

	return 0;
}
