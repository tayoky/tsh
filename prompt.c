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

char **history = NULL;
int history_len = 0;
int history_index = 0;

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
}

static void reset(char *line,int *cursor,int *len){
	//reset cursor pos
	move(-*cursor);
	*cursor = 0;

	for(int i=0; i<*len; i++){
		putchar(' ');
	}

	move(-*len);
	*len = 0;

	fflush(stdout);
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

static void save_history(char *line,int *cursor,int *len){
	if(history_index == history_len){
		return;
	}
	
	free(history[history_index]);
	history[history_index] = strndup(line,(*len));
}

static void load_history(char *line,int *cursor,int *len){
	reset(line,cursor,len);

	//special case for last entry
	if(history_index == history_len){
		reprint(line,*cursor,*len);
		return;
	}

	//load from history
	*len = strlen(history[history_index]);
	strcpy(line,history[history_index]);

	//set cursor to the end
	*cursor = strlen(line);
	move(*cursor);

	reprint(line,*cursor,*len);
}


static void parse_esc(char *line,int *cursor,int *len){
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
		if(*cursor >= *len){
			alert();
			break;
		}
		(*cursor)++;
		move(1);
		break;
	case 'A' :
		//up key
		if(history_index <= 0){
			alert();
			break;
		}
		save_history(line,cursor,len);
		history_index--;
		load_history(line,cursor,len);
		break;
	case 'B' :
		//down key
		if(history_index >= history_len - 1){
			alert();
			break;
		}
		save_history(line,cursor,len);
		history_index++;
		load_history(line,cursor,len);
		break;
	case 'H' :
		//home key
		//go to the start
		move(-*cursor);
		*cursor = 0;
		break;
	case 'F' :
		//end key
		move((*len)-(*cursor));
		*cursor = *len;
		break;
	default :
		putchar(c2);
	}
	fflush(stdout);
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

#endif

char *prompt(){
#ifndef NO_TERMIOS
	//init history if needed
	if(!history){
		history = malloc(1);
		history_len = 0;
	}
	history_index = history_len;
	//add a empty entry
	history_len++;
	history = realloc(history,history_len * sizeof(char *));
	history[history_index] = strdup("");
#endif

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

		switch(c){
		case '\033' :
			//espace sequence comming
			parse_esc(line,&cursor,&len);
			continue;
		case 0x7f :
			erase(line,&cursor,&len);
			reprint(line,cursor,len);
			continue;
		case '\t' :
			//TODO : auto completion
			//or something like that
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
	
	//save to last history entry
	history_index = history_len - 1 ;
	free(history[history_index]);
	history[history_index] = strdup(line);

	//remove from history if empty
	if(!line[0]){
		free(history[history_index]);
		history_len--;
		history = realloc(history,history_len * sizeof(char *));
	}
#endif

	return line;
}
