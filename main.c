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
			printf("tsh3 v0.1.0\n");
		}
		if(!strcmp(argv[1],"-c")){
			//execute one single command
			signal_control = 0;
			if(argc < 3){
				printf("tsh : -c option require one argument\n");
				return -1;
			}
			FILE *cmd = tmpfile();
			fputs(argv[2],cmd);
			rewind(cmd);
			return interpret(cmd);
		}
		return script_mode(argv[1]);
	}
}

int script_mode(const char *path){
	signal_control = 0;
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
