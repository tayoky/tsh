#include <stdlib.h>
#include <stdio.h>

extern char **environ;

int export(int argc,char **argv){
    int ret = 0;
    if(argc == 1){
        //print variables
        int i = 0;
        while(environ[i]){
            printf("%s\n",environ[i]);
            i++;
        }
    } else {
        for(int i=1;i<argc;i++){
            if(argv[i][0] == '='){
                printf("%s : not valid variable name\n",argv[i]);
                ret = -1;
                continue;
            }
            putenv(argv[i]);
        }
    }
    return ret;
}