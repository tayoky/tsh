#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "builtin.h"
#include "tsh.h"


//lock used by if else and that stuff
int lock = 0;

//list of built in command
struct cmd{
	char *name;
	int (*function)(int,char**);
	int bypass_lock;
};

struct cmd builtin[]= {
	{
		.name = "exit",
		.function = texit
	},{
		.name = "cd",
		.function = cd
	},{
		.name = "version",
		.function = version
	},{
		.name = "export",
		.function = export
	},{
		.name = "if",
		.function = _if,
		.bypass_lock = 1
	},{
		.name = "fi",
		.function = fi,
		.bypass_lock = 1
	},{
		.name = "then",
		.function = then
	}
};

#define S_ALL S_IRUSR | S_IWUSR

pid_t spawn(char **arg){
	//check for built in
	for(int i=0; i< sizeof(builtin) / sizeof(*builtin);i++){
		if(!strcmp(arg[0],builtin[i].name)){
			int argc = 0;
			while(arg[argc]){
				argc++;
			}
			int ret = builtin[i].function(argc,arg);

			pid_t child = fork();
			if(!child){
				//dummy child
				exit(ret);
			}
			return child;
		}
	}
			
	pid_t child = fork();
	if(!child){
		execvp(arg[0],arg);
		perror(arg[0]);
		exit(1);
	}
	return child;
}

int exec_line(char *line){
	//first tokenize
	token *tokens = tokenize(line);
	token *lexer = tokens;

	char ****top = parse(tokens);

	//now free the tokens
	token *tok = tokens; 
	while(tok->type){
		free(tok->value);
		tok++;
	}
	free(tok->value);
	free(tokens);

	int i = 0;
	int j = 0;
	int k = 0;
	while(top[i]){
		while(top[i][j]){
			spawn(top[i][j]);
			j++;
		}
		i++;
		while(j > 0){
			waitpid(-1,0,0);
			j--;
		}
	}

	return 0;
}
