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

#define syntax_error() {error("syntax error near token %s",token_name(current));\
	destroy_token(current);\
	if(prev)destroy_token(current);\
	return ast_cleanup(first_expr);}


//return the last arg, or create a new one if needed
AST_node *new_arg(token *prev,AST_node **last_arg,AST_node **last_cmd,AST_node **current_top){
	if(prev && prev->type != T_SPACE && *last_arg){
		return *last_arg;
	}

	AST_node *arg = new_node();
	arg->type = AST_ARG;
	arg->value = strdup("");

	if(*last_cmd){
		(*last_arg)->right = arg;
	} else {
		*last_cmd = new_node();
		(*last_cmd)->type = AST_COMMAND;
		(*current_top)->right = *last_cmd;
		(*last_cmd)->left = arg;
	}

	*last_arg = arg;

	return arg;
}

#define context prev,&last_arg,&last_cmd,&current_top
#define append(arg,str) arg->value = realloc(arg->value,strlen(arg->value)+strlen(str)+1);\
strcat(arg->value,str)

AST_node *parser(FILE *file){
	AST_node *first_expr = new_node();
	first_expr->type = AST_EXPR;
	AST_node *last_expr = first_expr;

	AST_node *current_top = last_expr;
	AST_node *last_cmd = NULL;
	AST_node *last_arg = NULL;

	token *prev = NULL;

	token *current = next_token(file);
	while(current){
		switch(current->type){
		case T_STR:;
			AST_node *arg = new_arg(context);
			append(arg,current->value);	
			break;
		case T_QUOTE:;
			//enter a litteral string
			AST_node *string = new_arg(context);
			destroy_token(current);
			current = next_token(file);
			while(current->type != T_QUOTE){
				if(current->type == T_EOF){
					syntax_error();
				} else {
					append(string,token2str(current));
				}
				destroy_token(current);
				current = next_token(file);
			}
			break;
		case T_DOLLAR:
			destroy_token(current);
			current = next_token(file);
			AST_node *str = new_arg(context);
			char buf[32];
			switch(current->type){
			case T_DOLLAR:
				sprintf(buf,"%d",getpid());
				append(str,buf);
				break;
			case T_STR:
				if(getenv(current->value)){
					append(str,getenv(current->value));
				}
				break;
			default:
				syntax_error();
			}
			break;
		case T_OR:
		case T_AND:
		case T_PIPE:
			if(!last_cmd)syntax_error();

			AST_node *op = new_node();
			switch(current->type){
			case T_OR:
				op->type = AST_OR;
				break;
			case T_AND:
				op->type = AST_AND;
				break;
			case T_PIPE:
				op->type = AST_PIPE;
				break;
			}

			op->left = current_top->right;

			//set the op at the right of the top node
			current_top->right = op;
			
			//setup new context
			current_top = op;
			last_arg = NULL;
			last_cmd = NULL;
			break;
		case T_EOF:
		case T_NEWLINE:
			goto finish;
		case T_SEMI_COLON:
			if(!last_cmd)syntax_error();
			AST_node *new_expr = new_node();
			new_expr->type = AST_EXPR;
			last_expr->left = new_expr;
			last_expr = new_expr;
			current_top = new_expr;
			last_arg = NULL;
			last_cmd = NULL;
			break;
		case T_INFERIOR:
		case T_SUPERIOR:
			destroy_token(current);

			//go to the next
			current = next_token(file);
			//TODO : create file ??? change flags on AST node ???
			break;
		case T_HASH:
			while(current->type != T_NEWLINE && current->type != T_EOF){
				destroy_token(current);
				current = next_token(file);
			}
			break;
		default:
			break;
		}
		if(prev)destroy_token(prev);
		prev = current;
		current = next_token(file);
	}
finish:
	if(prev)destroy_token(prev);
	if(current)destroy_token(current);

	return first_expr;
}
