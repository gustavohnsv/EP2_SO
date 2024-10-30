PROGRAM = mergesort

build:
	gcc src/${PROGRAM}.c -o ${PROGRAM}.exe -Wall -Werror -pthread -std=c11 -O

run: build
	./${PROGRAM}.exe 

clean:
	rm -f *.o *.exe *.txt *.csv *.out