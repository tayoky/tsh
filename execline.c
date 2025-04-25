#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tsh.h"

static int start(cmd *command,int out,int in,int del){
	pid_t child = fork();
	if(!child){
		if(del){
			close(del);
		}
		dup2(out,STDOUT_FILENO);
		dup2(in ,STDIN_FILENO);
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
		int pipefd[2];
		out = STDOUT_FILENO;

		//if there are something after us
		//we create a pipe and use it
		if(cur->next){
			pipe(pipefd);
			out = pipefd[1];
			start(cur,out,in,pipefd[0]);
		} else {
			start(cur,out,in,0);
		}

		//do some cleanup if needed
		if(in != STDIN_FILENO){
			close(in);
		}

		in = STDIN_FILENO;

		//if there are somthing after us
		//we connect the stdin of the next command
		if(cur->next){
			in = pipefd[0];
			close(pipefd[1]);
		}

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
	tokens = simplifier(tokens);
	token *cur = tokens;
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

	chain *chains = parser(tokens);
	chain *cur_chain = chains;
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
	cur_chain = chains;
	for(;cur_chain;cur_chain = cur_chain->next){
		execute(cur_chain);
	}
	return 0;
}
