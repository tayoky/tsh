# tsh
> [!NOTE] this branch is a rewrite  
> some feature from the main branch might not be here

tsh is a minimalist shell that focus on simplicity and portability
# features
features of the `rewrite` branch :
- basic lexer and parser
- pipes
- line editing
- history

# lack of features
features availible on the `main` branch but not on the rewrite :
- built in commands
- environement variable management
- output redirection
- cd command
- if statement

# build
to build just
```sh
git clone https://github.com/tayoky/tsh
cd tsh
./configure
make```

> [!NOTE] to compile wihout line editing and termios operation  
> run `./configure --no-termios`
and then run with
```sh
./tsh
```
