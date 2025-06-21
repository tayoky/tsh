#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "tsh.h"


//initialize the environement and signal
void init(int argc,char **argv){
	if(argc < 1)return;
	//setup $TSH
	char *tsh = malloc(strlen(argv[0]) + 5);
	sprintf(tsh,"TSH=%s",argv[0]);
	putenv(tsh);

	//we don't want to be stopped
	if(signal_control){
		signal(SIGTTOU,SIG_IGN);
	}
}
