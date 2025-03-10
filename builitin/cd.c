#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

//cd built in
int cd(int argc,char **argv){
	if(argc == 1){
		char *home = getenv("HOME");
		if(!home){
			chdir("/");
			return 0;
		} else {
			chdir(home);
			return 0;
		}
	} else if(argc == 2){
		int code = chdir(argv[1]);
		if(code != 0){
			printf("cd : %s\n",strerror(errno));
		}
		return 0;
	} else {			printf("cd too many arguments\n");
		return -1;
	}
}
