#include <stdio.h>
#include <stdlib.h>

//exit built in
int texit(int argc,char **argv){		
	if(argc == 1){			
		exit(0);
	} else if(argc == 2){
		int code = atoi(argv[1]);
		if(!code){
			printf("exit : exit code must be a numeric number\n");
			return -1;
		}
		exit(code);
	} else {
		printf("exit : too many arguments\n");
		return -1;
	}
}
