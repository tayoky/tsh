#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "tsh.h"
#include "malloc-check.h"

typedef struct exec_context {
	int in;
	int out;
} exec_context;

int lock = 0;
int exit_code;
int running_cmd = 0;

int wait_cmd(){
	int status = 0;
	while(running_cmd > 0){
		wait(&status);
		running_cmd--;
	}
	return status;
}

int launch(exec_context *context,char *const*args){
	//check for build it
	for(int i=0; i<3; i++){
		if(!strcmp(args[0],builtin_cmd[i].name)){
			int argc =0;
			while(args[argc])argc++;
			return builtin_cmd[i].func(argc,(char**)args);
		}
	}
	pid_t child = fork();
	if(!child){
		dup2(context->in,STDIN_FILENO);
		dup2(context->out,STDOUT_FILENO);
		execvp(args[0],args);
		error("%s : %s",args[0],strerror(errno));
		exit(1);
	}
	if(child < 0){
		error("fork : %s",strerror(errno));
		return 1;
	}
	
	running_cmd++;
	return 0;
}

static void fd_cleanup(int fd){
	if(fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO){
		close(fd);
	}
}

int execute_node(AST_node *node,exec_context *context){
	exec_context default_context;
	if(!context){
		context = &default_context;
		context->in = STDIN_FILENO;
		context->out = STDOUT_FILENO;
	}
		
	switch(node->type){
	case AST_EXPR:;
		if(!node->right)return 0;
		int expr_status = execute_node(node->right,context);
		wait_cmd();
		if(node->left){
			expr_status = execute_node(node->left,context) || wait_cmd();
		}
		return expr_status;
	case AST_AND:;
		int and_status = execute_node(node->left,context);
		if(and_status)return and_status;
		and_status = wait_cmd();
		if(and_status)return and_status;
		and_status = execute_node(node->right,context);
		if(and_status)return and_status;
		return wait_cmd();
	case AST_OR:;
		int or_status = execute_node(node->left,context) || wait_cmd();
		if(!or_status)return or_status;
		return execute_node(node->right,context) || wait_cmd();
	case AST_COMMAND:;
		int argc = 0;
		for(AST_node *cur = node->left;cur;cur = cur->right){
			argc++;
		}
		char **argv = malloc(sizeof(char *) * (argc + 1));
		argv[argc] = NULL;
		int i=0;
		for(AST_node *cur = node->left;cur;cur = cur->right){
			argv[i] = cur->value;
			i++;
		}

		int status = launch(context,argv);
		free(argv);
		return status;
	case AST_PIPE:;
		exec_context left = *context;
		exec_context right = *context;
		int pipefd[2];
		if(pipe(pipefd) < 0){
			error("pipe : %s",strerror(errno));
			return 1;
		}
		left.out = pipefd[1];
		right.in = pipefd[0];
		execute_node(node->left,&left);
		fd_cleanup(left.out);
		if(node->right)execute_node(node->right,&right);

		fd_cleanup(right.in);
		wait_cmd();
	}
	return 0;
}

#ifdef DEBUG
void print_node(AST_node *node,int depth){
	printf("%*s",depth,"");
	switch(node->type){
	case AST_AND:
		puts("and");
		break;
	case AST_OR:
		puts("or");
		break;
	case AST_PIPE:
		puts("pipe");
		break;
	case AST_ARG:
		printf("arg \"%s\"\n",node->value);
		break;
	case AST_COMMAND:
		puts("command");
		break;
	case AST_EXPR:
		puts("expression");
		break;
	}

	if(node->left)print_node(node->left,depth+1);
	if(node->right)print_node(node->right,depth+1);
}
#endif

int interpret(const char *text){
	start_malloc_check();
#ifdef DEBUG
	{
	const char *p = text;
	token *cur = next_token(&p);
	while(cur){
		if(cur->type == T_STR)
		printf("string %s\n",cur->value);
		else
		printf("token %s\n",token_name(cur));
		destroy_token(cur);
		cur = next_token(&p);
	}
	}
#endif

	AST_node *root = parser(text);
	if(!root){
#ifdef DEBUG
		malloc_check();
#endif
		return 1;
	}
#ifdef DEBUG
	print_node(root,0);
#endif

	execute_node(root,NULL);

	ast_cleanup(root);
#ifdef DEBUG
	malloc_check();
#endif
	return 0;
}
