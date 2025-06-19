#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tsh.h"


//initialize the environement
void init(int argc,char **argv){
	if(argc < 1)return;
	//setup $TSH
	char *tsh = malloc(strlen(argv[0]) + 5);
	sprintf(tsh,"TSH=%s",argv[0]);
	putenv(tsh);
}
