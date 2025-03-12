include config.mk

SRC = $(shell find ./ -name "*.c")
OBJ = ${SRC:.c=.o}
CFLAGS += -I ./include

all : tsh
tsh : ${OBJ}
	${CC} ${OBJ} -o tsh
%.o : %.c
	${CC} ${CFLAGS} -o $@ -c $^
clean :
	rm ${OBJ}
install : all
	cp tsh ${PREFIX}/bin
config.mk :
	$(error "run ./configure before runing make")
