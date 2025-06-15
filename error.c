#include <stdio.h>
#include <stdarg.h>
#include "tsh.h"

void error(const char *fmt,...){
	va_list args;
	va_start(args,fmt);
	fprintf(stderr,"tsh : ");
	vfprintf(stderr,fmt,args);
	fputc('\n',stderr);
	va_end(args);
}
