#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tsh.h"


//initialize the environement
void init(int argc,char **argv){
	//setup $TSH
	char *tsh = malloc(strlen(argv[0]) + 5);
	sprintf(tsh,"TSH=%s",argv[0]);
	putenv(tsh);
	
	//setup argument
	//for use in scripts
	int n = 0;
	for(int i=1;i<argc;i++){
		if(argv[i][0] == '-')continue;
		char *arg = malloc(strlen(argv[i]) + 64);
		sprintf(arg,"%d=%s",n,argv[i]);
		putenv(arg);
		n++;
	}
}
