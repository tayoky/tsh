#include <stdlib.h>
#include <string.h>
#include "tsh.h"

#define ERROR(...) {fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\n");return NULL;}

static void del(token **list,token *prev,token *tok){
	if(prev){
		prev->next = tok->next;
	} else {
		*list = tok->next;
	}
	free(tok);
}

static void add(token *prev,token *new){
	new->next = prev->next;
	prev->next = new;
}

token *simplifier(token *tokens){
	token *prev = NULL;
	for(token *current = tokens;current->type;current = current->next){
		token fake;
		fake.next = current->next;
		switch(current->type){
		case T_SPACE:
			del(&tokens,prev,current);
			current = &fake;
			break;
		case '$':
			if(current->next->type != T_STR){
				ERROR("syntax error near $");
			}
			char *var = strdup(current->next->value);
			del(&tokens,current,current->next);
			del(&tokens,prev,current);
			char *value = getenv(var);
			if(!value){
				fake.next = prev->next;
				current = &fake;
				free(var);
				break;
			}
			token *tok_val = malloc(sizeof(token));
			tok_val->type = T_STR;
			tok_val->value = strdup(value);
			add(prev,tok_val);
			current = tok_val;
			free(var);
			break;
		}
		prev = current;
	}
	return tokens;
}
