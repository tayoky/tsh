#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "tsh.h"
#include "malloc-check.h"

int lock = 0;
int exit_code;

int launch(char *const*args){
	pid_t child = fork();
	if(!child){
		execvp(args[0],args);
		error("%s : %s",args[0],strerror(errno));
		exit(1);
	}
	if(child < 0){
		error("fork : %s",strerror(errno));
		return 1;
	}
	int status;
	if(waitpid(child,&status,0) < 0){
		error("waitpid : %s",strerror(errno));
		return 1;
	}
	
	exit_code = status;
	return status;
}

int execute_node(AST_node *node){
	switch(node->type){
	case AST_EXPR:
		return execute_node(node->right);
	case AST_AND:;
		int and_status = execute_node(node->left);
		if(and_status)return and_status;
		return execute_node(node->right);
	case AST_OR:;
		int or_status = execute_node(node->left);
		if(!or_status)return or_status;
		return execute_node(node->right);
	case AST_COMMAND:;
		int argc = 0;
		for(AST_node *cur = node->left;cur;cur = cur->right){
			argc++;
		}
		char **argv = malloc(sizeof(char *) * (argc + 1));
		argv[argc] = NULL;
		int i=0;
		for(AST_node *cur = node->left;cur;cur = cur->right){
			argv[i] = cur->value;
			i++;
		}

		int status = launch(argv);
		free(argv);
		return status;
	}
	return 0;
}


void print_node(AST_node *node,int depth){
	printf("%*s",depth,"");
	switch(node->type){
	case AST_AND:
		puts("and");
		break;
	case AST_OR:
		puts("or");
		break;
	case AST_ARG:
		printf("arg \"%s\"\n",node->value);
		break;
	case AST_COMMAND:
		puts("command");
		break;
	case AST_EXPR:
		puts("expression");
		break;
	}

	if(node->left)print_node(node->left,depth+1);
	if(node->right)print_node(node->right,depth+1);
}

int exec_line(char *line){
	start_malloc_check();
	token *tokens = lexer(line);
	if(!tokens){
		goto ret;
	}
	{
	token *cur = tokens;
	while(cur){
		if(cur->type == T_STR)
		printf("string %s\n",cur->value);
		else
		printf("token %s\n",token_name(cur));
		cur = cur->next;
	}
	}

	AST_node *root = parser(tokens);
	if(!root){
		goto tokens_cleanup;
	}
	print_node(root,0);

	execute_node(root);

ast_cleanup:
	ast_cleanup(root);
tokens_cleanup:
	while(tokens){
		if(tokens->type == T_STR){
			free(tokens->value);
		}
		token *next = tokens->next;
		free(tokens);
		tokens = next;
	}
ret:
	malloc_check();
	return 0;
}
