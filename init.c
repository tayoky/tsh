#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tsh.h"


//initialize the environement
void init(int argc,char **argv){
	char *tsh = malloc(strlen(argv[0]) + 5);
	sprintf(tsh,"TSH=%s",argv[0]);
	putenv(tsh);
	free(tsh);
}
