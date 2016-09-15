
all: flex gcc
	@echo "Done."

flex: scanner.l
	flex scanner.l

gcc: scanner.c tiny.c
	gcc -Wall -o tiny scanner.c tiny.c

clean:
	@rm -f *.o scanner.c tiny
