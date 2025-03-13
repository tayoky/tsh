#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "builtin.h"


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
#define OUT(c) argv[argc-1][arg_size] = c;\
		arg_size++;\
		argv[argc-1] = realloc(argv[argc-1],arg_size + 1);\
		argv[argc-1][arg_size] = '\0'

char **parse_line(char *line,int *out){
	char prev_was_space = 1;
	char in_string = 0;
	char prev_was_backslash = 0;
	int argc = 0;
	char **argv = malloc(1);
	size_t arg_size = 0;

	for(int i=0;line[i];i++){
		if(line[i] == '\n'){
			break;
		}
		if(prev_was_backslash){
			OUT(line[i]);
			prev_was_backslash = 0;
			continue;
		}
		if((line[i] == ' ' || line[i] == '\t') && !in_string){
			prev_was_space = 1;
			continue;
		}

		//check if we just start a new arg
		if(prev_was_space){
			prev_was_space = 0;
			argv = realloc(argv,(argc + 1) * sizeof(char *));
			argv[argc] = malloc(1);
			argv[argc][0] = '\0';
			argc++;
			arg_size = 0;
		}

		if(line[i] == '"'){
			in_string = 1-in_string;
			continue;
		}
		if(line[i] == '$'){
			i++;
			char end = ' ';
			if(line[i] == '{'){
				i++;
				end = '}';
			}
			char *start = &line[i];
			size_t len = 0;
			while(line[i] != '\n' && line[i] != end){
				i++;
				len++;
			}
			if(line[i] != '{'){
				i--;
			}
			char *name = strndup(start,len);
			char *value = getenv(name);
			free(name);
			if(!value){
				continue;
			}
			for(int j=0;value[j];j++){
				OUT(value[j]);
			}
			continue;
		}
		if(line[i] == '\\'){
			prev_was_backslash = 1;
			continue;
		}

		OUT(line[i]);
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
			//if locked check
			if(lock && !builtin[i].bypass_lock){
				return 0;
			}
			return builtin[i].function(argc,argv);
		}
	}

	if(lock){
		return 0;
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
