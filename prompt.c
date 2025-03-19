#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *prompt(){
	char *line = malloc(256);
	fgets(line,255,stdin);
	if(strchr(line,'\n')){
		*strchr(line,'\n') = '\0';
	}
	return line;
}
