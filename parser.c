#include "tsh.h"
#include <stdlib.h>
#include <string.h>


//architecure
//top domain line
//	command line
//		single command
//			argument

char **last_argument;
char ***last_command;
char ****last_command_line;

static void new_command_line(char *****top){
	int i = 0;
	while((*top)[i]){
		i++;
	}

	*top = realloc(*top,(i + 2) * sizeof(char ***));
	last_command_line = &(*top)[i];
	(*top)[i] = malloc(sizeof(char **));
	(*top)[i][0] = NULL;

	(*top)[i+1] = NULL;

}

static void new_command(){
	int i = 0;
	while((*last_command_line)[i]){
		i++;
	}

	*last_command_line = realloc(*last_command_line,(i + 2) * sizeof(char **));
	last_command = &(*last_command_line)[i];
	(*last_command_line)[i] = malloc(sizeof(char *));
	(*last_command_line)[i][0] = NULL;

	(*last_command_line)[i+1] = NULL;
}

static void new_argument(){
	int i = 0;
	while((*last_command)[i]){
		i++;
	}

	*last_command = realloc(*last_command,(i + 2) * sizeof(char *));
	last_argument = &(*last_command)[i];
	(*last_command)[i] = malloc(sizeof(char));
	(*last_command)[i][0] = '\0';

	(*last_command)[i+1] = NULL;
}

static void add_to_arg(char *str){
	*last_argument = realloc(*last_argument,strlen(*last_argument) + strlen(str) + 1);
	strcat(*last_argument,str);
}

char ****parse(token *tokens){
	char ****top = malloc(sizeof(char ***));
	top[0] = NULL;
	int prev_is_sep = 1;
	int prev_is_pipe = 1;
	while(tokens->type){
		if(prev_is_sep){
			new_command_line(&top);
			prev_is_sep = 0;
		}
		if(prev_is_pipe){
			new_command();
			prev_is_pipe = 0;
		}
		switch(tokens->type){
		case T_SEP :
			prev_is_sep = 1;
			continue;
		case T_ARGSTART :
			new_argument();
			break;
		case T_STRING :
			add_to_arg(tokens->value);
			break;
		case T_VAR :
			//TODO: also check for local var?
			if(getenv(tokens->value)){
				add_to_arg(getenv(tokens->value));
			}
			break;
		}

		tokens++;
	}

	return top;
}
