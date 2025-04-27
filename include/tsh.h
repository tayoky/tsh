#ifndef TSH_H
#define TSH_H

typedef struct token {
	int type;
	char *value;
	struct token *next;
} token;

typedef struct redir {
	int fd;
	char *path;
	int flags;
} redir;

typedef struct cmd {
	int argc;
	char **argv;
	struct cmd *next;
	unsigned int flags;
} cmd;

typedef struct chain {
	struct cmd *commands;
	struct chain *next;
	struct redir *redirections;
} chain;

typedef struct builtin {
	int (*func)(int,char **);
	char *name;
	int lock_bypass;
} builtin;

#define T_NULL 0
#define T_STR  1
#define T_SPACE ' '

#define CMD_BG   0x01
#define CMD_PIPE 0x02
#define CMD_NULL 0x04

#define REDIR_IN  0x01
#define REDIR_OUT 0x02

extern int lock;

extern builtin builtin_cmd[6];

char *prompt();

int exec_line(char *line);

token *lexer(char *line);

token *simplifier(token *tokens);

chain *parser(token *tokens);

#endif
