
TARGET = fduf
OBJECTS = ${TARGET}.o getfiles.o fileinfo.o
MANFILE = ${TARGET}.1

all: ${TARGET}
	@echo finished building ${TARGET}

${TARGET}: ${OBJECTS}
	@echo linking ${TARGET}
	@gcc ${OBJECTS} -o ${TARGET} -lcrypto

.c.o:
	@echo compiling $*.c
	@gcc -c $*.c


clean:
	rm -f ${OBJECTS} ${TARGET}
