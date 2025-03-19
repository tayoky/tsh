#include "tsh.h"
#include <stdlib.h>
#include <string.h>


//architecure
//top domain line
//	command line
//		single command
//			argument

static void new_command_line(char *****top){
	int i = 0;
	while((*top)[i]){
		i++;
	}

	*top = realloc(*top,(i + 2) * sizeof(char ***));
	(*top)[i] = malloc(sizeof(char **));
	(*top)[i][0] = NULL;

	(*top)[i+1] = NULL;

}

char ****parse(token *tokens){
	char ****top = malloc(sizeof(char ***));
	top[0] = NULL;
	int prev_is_sep = 1;
	while(tokens->type){
		if(prev_is_sep){
			new_command_line(&top);
		}
		switch(tokens->type){
		case T_SEP :
			prev_is_sep = 1;
			continue;
		case T_ARGSTART :
			break;
		}

		tokens++;
	}
}
