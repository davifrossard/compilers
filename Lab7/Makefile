
all: flex bison gcc
	@echo "Done."

bison: parser.y
	bison -d parser.y
	
flex: scanner.l
	flex scanner.l

gcc: scanner.c parser.c
	gcc -Wall -o tiny scanner.c parser.c

run: all
	./tiny

test: all tiny
	for i in in/*.tny; do \
		echo $$i; \
		./tiny < $$i | diff out/$$(basename $$i .tny).out -; \
	done

clean:
	@rm -f tiny scanner.c parser.c parser.h 
