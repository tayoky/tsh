#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "tsh.h"
#include "malloc-check.h"

struct op {
	char *str;
	size_t len;
	int type;
};

#define OP(t,n) {.type = t,.str = n,.len = sizeof(n)-1}

//must be from bigger to smaller
struct op operators[]={
	OP(T_AND,"&&"),
	OP(T_OR,"||"),
	OP(T_BG,"&"),
	OP(T_PIPE,"|"),
	OP(T_OPEN_BRACK,"{"),
	OP(T_CLOSE_BRACK,"}"),
	OP(T_OPEN_PAREN,"("),
	OP(T_CLOSE_PAREN,")"),
	OP(T_SEMI_COLON,";"),
	OP(T_INFERIOR,"<"),
	OP(T_SUPERIOR,">"),
};


const char *token_name(token *t){
	for(int i=0; i<arraylen(operators); i++){
		if(operators[i].type == t->type){
			return operators[i].str;
		}
	}

	switch(t->type){
	case T_END:
		return "<end>";
	case T_STR:
		return "<string>";
	default:
		return "<unknow>";
	}
}


int get_operator(char *str){
	for(int i=0; i<arraylen(operators); i++){
		if(!memcmp(str,operators[i].str,operators[i].len)){
			return i;
		}
	}

	return -1;
}

char *skip_blank(char *str){
	while(isblank(*str)){
		str++;
	}
	return str;
}

token *new_token(token **first,token **last){
	token *new = malloc(sizeof(token));
	memset(new,0,sizeof(token));
	if(!*first) *first = new;
	if(*last) (*last)->next = new;
	*last = new;
	return new;
}

char *end_of_str(char *str){
	for(;;){
		if(!*str)break;
		if(isblank(*str))break;
		if(get_operator(str) >= 0)break;
		str++;
	}
	return str;
}

token *lexer(char *line){
	token *last = NULL;
	token *first = NULL;
	line = skip_blank(line);
	while(*line){
		token *new = new_token(&first,&last);
		int op = get_operator(line);
		if(op < 0){
			char *end = end_of_str(line);
			new->value = strndup(line,end - line);
			line = end;
			new->type = T_STR;
		} else {
			line += operators[op].len;
			new->type = operators[op].type;
		}
		line = skip_blank(line);
	}

	token *end = new_token(&first,&last);
	end->type = T_END;
	return first;
}
