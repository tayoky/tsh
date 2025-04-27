#include "tsh.h"


#define DEF(func) extern int func(int argc,char **argv)


DEF(cd);
DEF(texit);
DEF(export);
DEF(_if);
DEF(fi);
DEF(then);

builtin builtin_cmd[] = {
	{.lock_bypass=0,.func = cd    ,.name = "cd"},
	{.lock_bypass=0,.func = texit ,.name = "exit"},
	{.lock_bypass=0,.func = export,.name = "export"},
	{.lock_bypass=1,.func = _if   ,.name = "if"},
	{.lock_bypass=1,.func = fi    ,.name = "fi"},
	{.lock_bypass=0,.func = then  ,.name = "then"},
};
