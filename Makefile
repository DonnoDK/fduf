TARGET = fduf
OBJECTS = ${TARGET}.o getfiles.o fileinfo.o
MANFILE = ${TARGET}.1
CC = gcc

all: ${TARGET}
	@echo finished building ${TARGET}

${TARGET}: ${OBJECTS}
	@echo linking ${TARGET}
	@${CC} ${OBJECTS} -o ${TARGET} -lcrypto

.c.o:
	@echo compiling $*.c
	@${CC} -std=c99 -c $*.c


clean:
	rm -f ${OBJECTS} ${TARGET}
