#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tsh.h"
#include "malloc-check.h"

AST_node *new_node(void){
	AST_node *new = malloc(sizeof(AST_node));
	return memset(new,0,sizeof(AST_node));
}


AST_node *ast_cleanup(AST_node *node){
	if(node->left)ast_cleanup(node->left);
	if(node->right)ast_cleanup(node->right);
	if(node->type == AST_ARG){
		free(node->value);
	}
	free(node);
	return NULL;
}

AST_node *parser(token *current){
	AST_node *expr = new_node();
	expr->type = AST_EXPR;

	AST_node *last_cmd = new_node();
	last_cmd->type = AST_COMMAND;

	expr->right = last_cmd;
	AST_node *current_top = expr;

	AST_node *last_arg = NULL;

	while(current){
		switch(current->type){
		case T_END:
			break;
		case T_STR:;
			AST_node *arg = new_node();
			arg->type = AST_ARG;
			arg->value = strdup(current->value);
			if(last_arg){
				last_arg->right = arg;
			} else {
				last_cmd->left = arg;
			}
			last_arg = arg;
			break;
		case T_OR:
		case T_AND:;
			if(!last_arg){
				error("syntax error near token %s",token_name(current));
				return ast_cleanup(expr);
			}
			AST_node *op = new_node();
			switch(current->type){
			case T_OR:
				op->type = AST_OR;
				break;
			case T_AND:
				op->type = AST_AND;
				break;
			}

			op->left = current_top->right;

			//create a new cmd for the right
			last_cmd = new_node();
			last_cmd->type = AST_COMMAND;
			op->right = last_cmd;

			//set the op at the right of the top node
			current_top->right = op;
			
			//setup new context
			current_top = op;
			last_arg = NULL;
			break;
		default:
			break;
		}
		current = current->next;
	}

	return expr;
}
