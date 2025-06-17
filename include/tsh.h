#ifndef TSH_H
#define TSH_H

#include <stdlib.h>

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

typedef struct AST_node {
	int type;
	struct AST_node *left,*right;
	char *value;
} AST_node;

#define T_NULL         0
#define T_STR          1
#define T_AND          2
#define T_OR           3
#define T_EOF          4
#define T_PIPE        '|'
#define T_BG          '&'
#define T_SPACE       ' '
#define T_OPEN_BRACK  '{'
#define T_CLOSE_BRACK '}'
#define T_OPEN_PAREN  '('
#define T_CLOSE_PAREN ')'
#define T_SEMI_COLON  ';'
#define T_DQUOTE      '"'
#define T_QUOTE       '\''
#define T_INFERIOR    '<'
#define T_SUPERIOR    '>'
#define T_NEWLINE     '\n'

#define AST_NULL    0
#define AST_EXPR    1
#define AST_COMMAND 2
#define AST_ARG     3
#define AST_AND     4
#define AST_OR      5
#define AST_BG      6

#define arraylen(ar) (sizeof(ar)/sizeof(*ar))
extern int lock;

void error(const char *fmt,...);

int shell_mode(void);
int script_mode(const char *path);

extern builtin builtin_cmd[7];

void init(int argc,char **argv);

char *prompt();

int interpret(const char *text);

token *lexer(const char *text);
const char *token_name(token *);
const char *token2str(token *);

AST_node *parser(token *);
AST_node *ast_cleanup(AST_node *);

#endif
