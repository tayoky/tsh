#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef NO_TERMIOS
#include <termios.h>
#include <unistd.h>
#endif

#define ESC "\033"

#ifndef NO_TERMIOS

struct termios old,new;

void enable_raw(){
	tcgetattr(STDIN_FILENO,&old);
	new = old;
	new.c_lflag &= ~(ICANON | ECHO);

	tcsetattr(STDIN_FILENO,TCSANOW,&new);
}

void restore_term(){
	tcsetattr(STDIN_FILENO,TCSAFLUSH,&old);
}

static void alert(){
	putchar('\a');
	fflush(stdout);
}

//move the TERMINAL cursor
static void move(int mv){
	if(!mv)return;
	char c = 'C';
	if(mv < 0){
		c = 'D';
		mv = -mv;
	}

	printf(ESC"[%i%c",mv,c);
	fflush(stdout);
}

static void parse_esc(int *cursor,int len){
	//first read two char
	char c1 = getchar();
	char c2 = getchar();

	//ignore seqence wihout an [
	if(c1 != '['){
		return;
	}

	switch(c2){
	case 'D' :
		//right key
		if(*cursor <= 0){
			alert();
			break;
		}
		(*cursor)--;
		move(-1);
		break;
	case 'C' :
		//left key
		if(*cursor >= len){
			alert();
			break;
		}
		(*cursor)++;
		move(1);
		break;
	default :
		putchar(c2);
		fflush(stdout);
	}
}

static void erase(char *line,int *cursor,int *len){
	//check there are something to erase
	if(*cursor <= 0){
		alert();
		return;
	}
	//offset everything after the cursor
	
	memmove(&line[(*cursor)-1],&line[(*cursor)],(*len) - (*cursor));
	move(-1);
	(*cursor)--;
	(*len)--;
}

static void reprint(char *line,int cursor,int len){
	//go back to the start
	move(-cursor);

	for(int i=0; i<len; i++){
		putchar(line[i]);
	}

	//clear one more char (for erease)
	putchar(' ');
	move(-1);

	//go back to the end again
	move(-len);

	//restore cursor pos
	move(cursor);

	//render
	fflush(stdout);
}

#endif

char *prompt(){
	char *line = malloc(256);
#ifdef NO_TERMIOS
	fgets(line,255,stdin);
	if(strchr(line,'\n')){
		*strchr(line,'\n') = '\0';
	}
#else
	//for various reason
	//raw mode might cause sole stdout issue
	//so make sure everthing is send
	fflush(stdout);

	int cursor = 0;
	int len = 0;
	enable_raw();
	for(;;){
		char c = 0;
		if(read(STDIN_FILENO,&c,1) < 0){
			break;
		}

		if(c == '\n'){
			putchar('\n');
			fflush(stdout);
			break;
		}

		if(c == '\033'){
			//espace sequence comming
			parse_esc(&cursor,len);
			continue;
		}

		if(c == 0x7f){
			erase(line,&cursor,&len);
			reprint(line,cursor,len);
			continue;
		}

		//insert a char
		//first offset everything
		//after the cursor
		memmove(&line[cursor+1],&line[cursor],len - cursor);

		//acctually insert the char
		line[cursor] = c;

		//update cursor pos/lenght
		move(1);
		cursor++;
		len++;

		//now reprint
		reprint(line,cursor,len);
	}
	line[len] = '\0';
	restore_term();
	
#endif
	return line;
}
