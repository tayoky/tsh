#ifndef TSH_H
#define TSH_H

typedef struct token {
	int type;
	char *value;
	struct token *next;
} token;


typedef struct builtin {
	int (*func)(int,char **);
	char *name;
	int lock_bypass;
} builtin;

#define T_NULL         0
#define T_STR          1
#define T_AND          2
#define T_OR           3
#define T_END          4
#define T_PIPE        '|'
#define T_BG          '&'
#define T_SPACE       ' '
#define T_OPEN_BRACK  '{'
#define T_CLOSE_BRACK '}'
#define T_OPEN_PAREN  '('
#define T_CLOSE_PAREN ')'
#define T_SEMI_COLON  ';'

#define arraylen(ar) (sizeof(ar)/sizeof(*ar))

extern int lock;

int shell_mode(void);
int script_mode(const char *path);

extern builtin builtin_cmd[7];

void init(int argc,char **argv);

char *prompt();

int exec_line(char *line);

token *lexer(char *line);



#endif
