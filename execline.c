#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "tsh.h"

int lock = 0;


int exec_line(char *line){
	token *tokens = lexer(line);
	if(!tokens){
		goto ret;
	}
	while(tokens){
		if(tokens->type > 32)
		printf("token <%c>\n",tokens->type);
		else if(tokens->type == T_STR)
		printf("string %s\n",tokens->value);
		else
		printf("token <%d>\n",tokens->type);
		tokens = tokens->next;
	}
ret:
	return 0;
}
