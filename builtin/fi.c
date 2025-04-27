#include "tsh.h"


//fi builtin command

int fi(int argc,char **argv){
	if(lock > 0){
		lock--;
	}

	return 0;
}
