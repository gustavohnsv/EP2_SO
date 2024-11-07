PROGRAM = psort14713982

build:
	gcc src/${PROGRAM}.c -o ${PROGRAM}.exe -Wall -Werror -pthread -std=c11 -O

clean:
	rm -f *.o *.exe *.txt *.csv *.out