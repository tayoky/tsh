#include "tsh.h"


#define DEF(func) extern int func(int argc,char **argv)


DEF(cd);
DEF(texit);


builtin builtin_cmd[] = {
	{.func = cd    ,.name = "cd"},
	{.func = texit ,.name = "exit"},
};
