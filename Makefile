PROGRAM = psort14713982
CC = gcc
CFLAGS = -Wall -Werror -pthread -std=c11 -O
SRC_DIR = src

build:
	${CC} ${SRC_DIR}/${PROGRAM}.c -o ${PROGRAM}.exe ${CFLAGS}

debug: CFLAGS += -g
debug:
	${CC} ${SRC_DIR}/${PROGRAM}.c -o ${PROGRAM}.exe ${CFLAGS}

clean:
	rm -f *.o *.exe *.txt *.csv *.out