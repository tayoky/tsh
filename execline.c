#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tsh.h"

int lock = 0;

static int start(cmd *command,int out,int in,int del){
	//check for built in first
	for(int i=0;i < sizeof(builtin_cmd) / sizeof(builtin); i++){
		if(!strcmp(command->argv[0],builtin_cmd[i].name)){
			if(lock && !builtin_cmd[i].lock_bypass){
				return 0;
			}
			return builtin_cmd[i].func(command->argc,command->argv);
		}
	}

	if(lock){
		return 0;
	}

	pid_t child = fork();
	if(!child){
		if(del){
			close(del);
		}
#ifndef NO_REDIR
		dup2(out,STDOUT_FILENO);
		dup2(in ,STDIN_FILENO);
#endif
		if(out != STDOUT_FILENO){
			close(out);
		}
		if(in != STDIN_FILENO){
			close(in);
		}
		execvp(command->argv[0],command->argv);
		perror(command->argv[0]);
		exit(1);
	}
	return 0;
}

static void execute(chain *ch){
	//we just have to execute a chain
	size_t count = 0;
	cmd *cur = ch->commands;
	int in = STDIN_FILENO;
	int out = STDOUT_FILENO;
	while(cur){
#ifndef NO_PIPE
		int pipefd[2];
#endif
		out = STDOUT_FILENO;

		//if there are something after us
		//we create a pipe and use it
#ifndef NO_PIPE
		if(cur->next){
			pipe(pipefd);
			out = pipefd[1];
			start(cur,out,in,pipefd[0]);
		} else
#endif
		{
			start(cur,out,in,0);
		}

#ifndef NO_PIPE
		//do some cleanup if needed
		if(in != STDIN_FILENO){
			close(in);
		}
#endif
		in = STDIN_FILENO;

#ifndef NO_PIPE
		//if there are somthing after us
		//we connect the stdin of the next command
		if(cur->next){
			in = pipefd[0];
			close(pipefd[1]);
		}
#endif

		cur = cur->next;
		count++;
	}

	while(count){
		int status = 0;
		wait(&status);
		count--;
	}
}

int exec_line(char *line){
	token *tokens = lexer(line);
	if(!tokens){
		goto ret;
	}
	tokens = simplifier(tokens);
	if(!tokens){
		goto ret;
	}
	token *cur = tokens;
#ifdef DEBUG
	for(;cur->type;cur=cur->next){
		switch(cur->type){
			case T_NULL:
				break;
			case T_STR:
				printf("<str> : %s\n",cur->value);
				break;
			default:
				printf("<%c>\n",cur->type);
				break;
		}
	}
#endif

	chain *chains = parser(tokens);
	if(!chains){
		goto cleanup;
	}
	chain *cur_chain = chains;

#ifdef DEBUG
	for(;cur_chain;cur_chain = cur_chain->next){
		cmd *ccur = cur_chain->commands;
		printf("chain:\n");
		for(;ccur;ccur = ccur->next){
			printf("\tcmd:\n");
			for(int i=0;i<ccur->argc;i++){
				printf("\t\targ%d : %s\n",i,ccur->argv[i]);
			}
		}
	}
#endif

	cur_chain = chains;
	for(;cur_chain;cur_chain = cur_chain->next){
		execute(cur_chain);
	}
	
	//TODO : free chain
	cleanup:
	cur = tokens;
	while(cur){
		token *next = cur->next;
		if(cur->type == T_STR){
			free(cur->value);
		}
		free(cur);
		cur = next;
	}
	ret:
	return 0;
}
