#include "tsh.h"


#define DEF(func) extern int func(int argc,char **argv)


DEF(cd);
DEF(texit);
DEF(export);


builtin builtin_cmd[] = {
	{.func = cd    ,.name = "cd"},
	{.func = texit ,.name = "exit"},
	{.func = export,.name = "export"},
};
