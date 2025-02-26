#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "execline.h"

int shell_mode();
int script_mode(const char *path);

int main(int argc,char **argv){
	if(argc < 2){
		//shell mode
		shell_mode();
	} else {
		if(!strcmp(argv[1],"-c")){
			//execute one single command
			if(argc < 3){
				printf("tsh : -c option require one argument\n");
				return -1;
			}
			return exec_line(strdup(argv[2]));
		}
		script_mode(argv[1]);
	}
}

int script_mode(const char *path){
	FILE *file = fopen(path,"r");
	if(file == NULL){
		printf("%s : %s\n",path,strerror(errno));
		exit(-1);
	}

	char line[256];
	
	while(fgets(line,sizeof(line) - 1,file)){
		if(line[0] == '#'){
			//it's a comment
			//skip
			continue;
		}
		exec_line(line);
	}

	fclose(file);
	exit(0);
}

int shell_mode(){
	for(;;){
		//show the prompt
		char cwd[256];
		getcwd(cwd,255);
		printf("%s$ ",cwd);
		char line[128];
		fgets(line,128,stdin);


		//execute the line
		exec_line(line);
	}
}
