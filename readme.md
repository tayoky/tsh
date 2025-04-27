# tsh
> [!NOTE] this branch is a rewrite  
> some feature from the main branch might not be here

tsh is a minimalist shell that focus on simplicity and portability
# features
features of the `rewrite` branch :
- basic lexer and parser
- built in command such as `cd` and `exit`
- pipes
- line editing
- history
- environ management (export and variable with $)
- if statement (no `else` supported)

# lack of features
features availible on the `main` branch but not on the rewrite :
- output redirection

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
