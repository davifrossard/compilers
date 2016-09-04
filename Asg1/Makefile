
all: flex gcc
	@echo "Done."

flex: scanner.l
	flex scanner.l

gcc: scanner.c
	gcc -Wall -o trab1 scanner.c -ly

clean:
	@rm -f *.o *.output scanner.c parser.h parser.c trab1

run: all trab1
	./trab1

test: all trab1
	for i in in/*.cm; do \
		echo $$i; \
		./trab1 < $$i | diff out1/$$(basename $$i .cm).out -; \
	done
