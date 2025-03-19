#include "tsh.h"
#include <stdlib.h>
#include <string.h>

//helper function to append a new token
static token *append_token(token **list){
	//find lenght
	size_t len = 0;
	while((*list)[len].type){
		len++;
	}

	len++;

	*list = realloc(*list,(len + 1) * sizeof(token));
	(*list)[len-1].value = malloc(1);
	(*list)[len-1].value[0] = '\0';

	(*list)[len].type = T_NULL;

	return &(*list)[len-1];
}

//helper function to append a char to the last token
static void append_char(token **list,char c){
	//find the last token
	int i = 0;
	while((*list)[i].type){
		i++;
	}

	//should not be possible;
	assert(i > 0);

	char *str = (*list)[i-1].value;

	str = realloc(str,strlen(str)+2);
	str[strlen(str)+1] = '\0';
	str[strlen(str)]   = c;

	(*list)[i-1].value = str;
}

token *tokenize(const char *line){
	token *list = malloc(sizeof(token));
	list[0].type = T_NULL;
	int in_string = 0;
	int in_var = 0;
	int backslash = 0;
	int prev_is_space = 1;
	for(;*line;line++){
		if(!backslash)
		switch(*line){
		case '|' :
			if(in_string)break;
			append_token(&list)->type = T_PIPE;
			prev_is_space = 1;
			continue;
		case ' ' :
			if(in_string)break;
			prev_is_space = 1;
			continue;
		case '"' :
			in_string = 1 - in_string;
			continue;
		case '$' :
			in_var = 1;
			append_token(&list)->type = T_VAR;
			prev_is_space = 0;
			continue;
		case '{' :
			if(!in_var)break;
			in_var = 1;
			prev_is_space = 0;
			continue;
		case '}' :
			if(!in_var)break;
			in_var = 0;
			prev_is_space = 1;
			continue;
		case '#' :
			if(!in_string){
				return list;
			}
			break;
		case ';' :
			if(in_string)continue;
			append_token(&list)->type = T_SEP;
			prev_is_space = 1;
		case '\\' :
			backslash = 1;
			continue;
		}

		//normal case
		backslash = 0;
		if(prev_is_space){
			append_token(&list)->type = T_ARGSTART;
			append_token(&list)->type = T_STRING;
			prev_is_space = 0;
		}
		append_char(&list,*line);

	}
	return list;
}
