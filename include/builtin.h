#ifndef BUILTIN_H
#define BUILTIN_H

int version(int argc,char **argv);
int texit(int argc,char **argv);
int cd(int argc,char **argv);
int export(int argc,char **argv);
int _if(int argc,char **argv);
int fi(int argc,char **argv);
int then(int argc,char **argv);

#endif
