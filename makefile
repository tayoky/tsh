SRC = $(shell find ./ -name "*.c")
OBJ = ${SRC:.c=.o}
SYSROOT =

all : tsh
tsh : ${OBJ}
	gcc ${OBJ} -o tsh
%.o : %.c
	${CC} ${CFLAGS} -o $@ -g -c $^
clean :
	rm ${OBJ}
install : all
	cp tsh ${SYSROOT}/bin
