#include <stdio.h>
#include <unistd.h>
#include "execline.h"

int main(){
	for(;;){
		//show the prompt
		char cwd[256];
		getcwd(cwd,255);
		printf("%s$ ",cwd);
		char line[128];
		fgets(line,128,stdin);


		//execute the line
		exec_line(line);
	}
}
