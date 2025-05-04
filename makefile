include config.mk

SRC = $(shell find ./ -name "*.c")
OBJ = ${SRC:.c=.o}
CFLAGS += -I ./include
CFLAGS += -DHOST=\"${HOST}\"

all : tsh
tsh : ${OBJ}
	@echo "[linking into $@]"
	@${CC} ${OBJ} -o $@
%.o : %.c
	@echo "[compiling $^]"
	@${CC} ${CFLAGS} -o $@ -c $^
clean :
	rm ${OBJ}
install : all
	@echo "[installing into ${PREFIX}/bin/tsh]"
	@mkdir -p ${PREFIX}/bin
	@cp tsh ${PREFIX}/bin
test : tsh
	./test.sh
config.mk :
	$(error "run ./configure before runing make")
