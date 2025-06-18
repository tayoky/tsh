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
	OP(T_NEWLINE,"\r\n"),
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
	OP(T_NEWLINE,"\n"),
	OP(T_SPACE," "),
	OP(T_SPACE,"\t"),
	OP(T_QUOTE,"'"),
	OP(T_DQUOTE,"\""),
	OP(T_HASH,"#"),
};

const char *token2str(token *t){
	switch(t->type){
	case T_SPACE:
		return " ";
	case T_NEWLINE:
		return "\n";
	default:
		return token_name(t);
	}
}
const char *token_name(token *t){
	switch(t->type){
	case T_EOF:
		return "<eof>";
	case T_STR:
		return "<string>";
	case T_NEWLINE:
		return "<newline>";
	case T_SPACE:
		return "<space>";
	}

	for(size_t i=0; i<arraylen(operators); i++){
		if(operators[i].type == t->type){
			return operators[i].str;
		}
	}
	return "<unknow>";
}


static int get_operator(const char *str){
	for(size_t i=0; i<arraylen(operators); i++){
		if(!memcmp(str,operators[i].str,operators[i].len)){
			return i;
		}
	}

	return -1;
}

static const char *end_of_str(const char *str){
	for(;;){
		if(!*str)break;
		if(get_operator(str) >= 0)break;
		str++;
	}
	return str;
}

token *next_token(const char **p){
	if(!*p)return NULL;
	token *new = malloc(sizeof(token));
	memset(new,0,sizeof(token));

	//if aready at the end return EOF
	if(!**p){
		new->type = T_EOF;
		*p = NULL;
		return new;
	}
	
	int op = get_operator(*p);
	if(op < 0){
		const char *end = end_of_str(*p);
		new->value = strndup(*p,end - *p);
		*p = end;
		new->type = T_STR;
	} else {
		*p += operators[op].len;
		new->type = operators[op].type;
	}
	return new;
}

void destroy_token(token *t){
	free(t->value);
	free(t);
}
