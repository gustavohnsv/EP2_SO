PROGRAM = mergesort

build:
	gcc src/${PROGRAM}.c -o ${PROGRAM}.exe -lpthread

run: build
	./${PROGRAM}.exe 

clean:
	rm -f *.o *.exe *.txt