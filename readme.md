# tsh
tsh is the minimalist shell created by tayoky for the stanix operating system  
## goal
the goal of tsh is to provide basic functionality (eg shell scripting ,pipes , ...) while still being very portable
## build
first run the configure script
```sh
./configure
```
then compile
```sh
make
```
and then launch 
```sh
./tsh
```
## installing
installing localy
```sh
./configure --prefix="$HOME/bin"
make
make install
```
installing globaly
```sh
./configure --prefix="/usr"
make
make install
```
