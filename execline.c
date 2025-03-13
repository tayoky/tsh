#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "builtin.h"

//list of built in command
struct cmd{
	char *name;
	int (*function)(int,char**);
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
	}
};

#define S_ALL S_IRWXU | S_IRWXG | S_IRWXO

char **parse_line(char *line,int *out){
	char prev_was_space = 1;
	char in_string = 0;
	int argc = 0;
	char **argv = malloc(1);
	size_t arg_size = 0;

	for(int i=0;line[i];i++){
		if(line[i] == '\n'){
			line[i] = '\0';
			break;
		}
		if(line[i] == ' ' && !in_string){
			line[i] = '\0';
			prev_was_space = 1;
			continue;
		}

		//check if we just start a new arg
		if(prev_was_space){
			prev_was_space = 0;
			argv = realloc(argv,(argc + 1) * sizeof(char *));
			argv[argc] = malloc(1);
			argc++;
			arg_size = 0;
		}

		if(line[i] == '"'){
			in_string = 1-in_string;
			continue;
		}

		argv[argc-1][arg_size] = line[i];
		arg_size++;
		argv[argc-1] = realloc(argv[argc-1],arg_size + 1);
		argv[argc-1][arg_size] = '\0';
	}

	//add the NULL at the end
	argv = realloc(argv,(argc + 1) * sizeof(char *));
	argv[argc] = NULL;

	*out = argc;
	return argv;
}

int exec_line(char *line){
	//first parse the line
	int argc = 0;
	char **argv = parse_line(line,&argc);

	//empty command case
	if(!argc){
		return 0;
	}

	//first check for built in command(exit,cd)
	int builtin_count = sizeof(builtin) / sizeof(builtin[0]);
	for(int i=0;i<builtin_count;i++){
		if(!strcmp(argv[0],builtin[i].name)){
			return builtin[i].function(argc,argv);
		}
	}

	int stdout_fd = STDOUT_FILENO;

	//scan for stdout redirection (> and >>)
	if(argc > 2){
		if(!strcmp(argv[argc-2],">")){
			stdout_fd = creat(argv[argc-1],S_ALL);

			argc-=2;
			argv[argc] = NULL;
		} else if (!strcmp(argv[argc-2],">>")){
			stdout_fd = open(argv[argc-1],O_CREAT | O_APPEND | O_WRONLY,S_ALL);

			argc -= 2;
			argv[argc] = NULL;
		}

		if(stdout_fd < 0){
			printf("%s : %s\n",argv[argc-1],strerror(errno));
			return -1;
		}
	}

	//fork and then execute
	pid_t child = fork();
	if(child < 0){
		printf("fork : %s\n",strerror(errno));
		return -1;
	}

	if(child == 0){
		//child code

		//copy stdout
		dup2(stdout_fd,STDOUT_FILENO);

		//exec
		execvp(argv[0],argv);
		printf("%s : %s\n",argv[0],strerror(errno));
		exit(errno);
	}

	int status;
	waitpid(child,&status,0);
	

	return 0;
}
