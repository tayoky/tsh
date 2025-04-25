#include <stdlib.h>
#include "tsh.h"

static void del(token **list,token *prev,token *tok){
	if(prev){
		prev->next = tok->next;
	} else {
		*list = tok->next;
	}
	free(tok);
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
		}
		prev = current;
	}
	return tokens;
}
