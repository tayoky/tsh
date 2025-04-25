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
		} else {
			chdir(home);
		}
	} else if(argc == 2){
		int code = chdir(argv[1]);
		if(code != 0){
			printf("cd : %s\n",strerror(errno));
			return errno;
		}
	} else {			printf("cd too many arguments\n");
		return -1;
	}

	//update $PWD
	char cwd[256];
	getcwd(cwd,255);
	char buf[256];
	snprintf(buf,256,"PWD=%s",cwd);
	putenv(buf);
	return 0;
}
