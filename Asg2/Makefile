# Compilador
CC = gcc
CC_FLAGS = -w -Wall -lm
BISONFILES = $(wildcard *.y)
BISON = $(BISONFILES:.y=.c)
FLEXFILES = $(wildcard *.l)
FLEX = $(FLEXFILES:.l=.c)
EXEC = cminus

all: $(EXEC)
	@echo "All done"
	
$(EXEC): $(FLEX) $(BISON)
	$(CC) $(FLEX) $(BISON) $(CC_FLAGS) -o $(EXEC)

%.c: %.y
	bison -d $^ -v
	@echo "Parser done"

%.c: %.l
	flex $^
	@echo "Scanner done"

clean:
	rm -f $(BISON) $(FLEX) $(EXEC) $(BISON:.c=.h) $(BISON:.c=.output)

run: $(EXEC)
	./$(EXEC)

test: $(EXEC)
	for i in in/*; do \
		echo $$i; \
		./$(EXEC) < $$i | diff out/$$(basename $$i .cm).out -; \
	done

