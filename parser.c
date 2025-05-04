#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "tsh.h"

#define ERROR(...) fprintf(stderr,"tsh : ");fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\n");goto error

static chain *new_chain(chain **last_chain,cmd **last_cmd){
	chain *new_chain = malloc(sizeof(chain));
	cmd   *new_cmd   = malloc(sizeof(cmd));
	memset(new_chain,0,sizeof(chain));
	memset(new_cmd  ,0,sizeof(cmd));
	new_cmd->argv = malloc(1);
	new_chain->commands = new_cmd;

	(*last_chain)->next = new_chain;
	
	(*last_chain) = new_chain;
	(*last_cmd)   = new_cmd;

	return new_chain;
}
static cmd *new_cmd(cmd **last){
	cmd *new = malloc(sizeof(cmd));
	memset(new,0,sizeof(cmd));
	new->argv = malloc(1);
	(*last)->next = new;
	*last = new;
	return new;
}

static char **new_arg(cmd **last){
	(*last)->argv = realloc((*last)->argv,((*last)->argc + 2) * sizeof(char *));
	char **arg = &(*last)->argv[(*last)->argc];
	*arg = malloc(1);
	(*last)->argc++;
	(*last)->argv[(*last)->argc] = NULL;
	return arg;
}

static redir *new_redir(chain *last_chain){
	redir *new = malloc(sizeof(redir));
	memset(new,0,sizeof(redir));
	new->next = last_chain->redirections;
	last_chain->redirections = new;
	return new;
}

chain *parser(token *tokens){
	//create a sub at the start
	//to make the everyrhing else simpler
	chain *first = malloc(sizeof(chain));
	chain *last = first;
	first->next = NULL;
	cmd *last_cmd = NULL;

	char is_first = 1;
	char must_create_chain = 1;

	token *current = tokens;
	token *prev = NULL;
	for(;current->type;current = current->next){
		switch(current->type){
		case '|':
#ifdef NO_PIPE
			ERROR("tsh was compiled with NO_PIPE");
#else
			if(current->next->type != T_STR){
				ERROR("syntax error near |");
			}
			new_cmd(&last_cmd);
			break;
#endif
		case '>':
			if(current->next->type != T_STR){
				ERROR("syntax error near >");
			}
#ifdef NO_REDIR
			ERROR("tsh was compiled with NO_REDIR");
#else
			redir *out_redir = new_redir(last);
			out_redir->flags = REDIR_OUT;
			out_redir->fd = STDOUT_FILENO;
			out_redir->path = current->next->value;
			current = current->next;
			break;
#endif
		case '&':
			if(is_first ||  (prev->type != T_STR)){
				ERROR("syntax error near &");
			}
			last_cmd->flags |= CMD_BG;
			must_create_chain = 1;
			break;
		case T_SPACE:
			is_first = 0;
			continue;
		case T_STR:
			//create a new chain
			if(must_create_chain){
				must_create_chain = 0;
				new_chain(&last,&last_cmd);
			}
			char **arg = new_arg(&last_cmd);
			*arg = realloc(*arg,strlen(current->value) + 1);
			strcpy(*arg,current->value);
			break;
		}
		is_first = 0;
		prev = current;
	}

	last->next = NULL;
	chain *old = first;
	first = old->next;
	free(old);
	return first;

	error:
	//TODO:also free redir
	while(first){
		while(first->commands){
			cmd *prev = first->commands;
			first->commands = prev->next;
			free(prev);
		}
		chain *prev = first;
		first = first->next;
		free(prev);
	}
	return NULL;
}
