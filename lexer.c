#include <stdlib.h>
#include <string.h>
#include "tsh.h"


token *lexer(char *line){
	token *first  = malloc(sizeof(token));
	token *last = first;
	last->type = T_NULL;
	
	char in_string = 0;
	char prev_backslash = 0;

	for(;*line;line++){
		//a backslash forces a default case
		if(prev_backslash){
			prev_backslash = 0;
			goto default_case;
		}
		switch(*line){
		case '$':
		case '{':
		case '}':
		case '(':
		case ')':
		case '<':
		case '>':
		case '|':
			//if in string jist use default case
			if(in_string)goto default_case;

			create_char_token:
			token *new_tok = malloc(sizeof(token));
			new_tok->type = *line;
			last->next = new_tok;
			last = new_tok;
			break;
		case ' ':
			//if in string just use default case
			if(in_string)goto default_case;
			//we add a new space token
			//but only if the last one isen't
			//aready a space
			if(last->type == T_SPACE)break;
			token *space_tok = malloc(sizeof(token));
			space_tok->type = T_SPACE;
			last->next = space_tok;
			last = space_tok;
			break;
		case '&':
			if(in_string)goto default_case;
			if(line[1] == '&'){
				//TODO create a && token
				break;
			}
			goto create_char_token;
			break;
		case '#':
			if(in_string)goto default_case;
			//we start a comment
			//stop tokenize now
			goto end_lexer;
		case '\\':
			prev_backslash= 1;
			break;
		case '"':
			//toggle between inside and outside string
			in_string = 1 - in_string;
			break;
		default:
			default_case:
			if(last->type != T_STR){
				token *str_tok = malloc(sizeof(token));
				str_tok->type = T_STR;
				str_tok->value = strdup("");
				last->next = str_tok;
				last = str_tok;
			}

			//append to the last str token
			last->value = realloc(last->value,strlen(last->value) + 2);
			last->value[strlen(last->value)+1] = '\0';
			last->value[strlen(last->value)] = *line;
			break;
		}
	}

	end_lexer:

	//add a NULL token a the end
	token *null_tok = malloc(sizeof(token));
	null_tok->type = T_NULL;
	null_tok->next = NULL;
	last->next = null_tok;
	last = null_tok;

	//remove the first token it's not needed anymore
	token *old_tok = first;
	first = old_tok->next;
	free(old_tok);

	return first;
}
