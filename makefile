SRC = $(shell find ./ -name "*.c")
OBJ = ${SRC:.c=.o}

all : tsh
tsh : ${OBJ}
	gcc ${OBJ} -o tsh
%.o : %.c
	${CC} ${CFLAGS} -o $@ -c $^

