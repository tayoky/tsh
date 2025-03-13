#include "execline.h"


//fi builtin command

int fi(int argc,char **argv){
	if(lock > 0){
		lock--;
	}
}
