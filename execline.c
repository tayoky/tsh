#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "tsh.h"

int lock = 0;

void malloc_check();

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
	malloc_check();
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
