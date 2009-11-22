CC = gcc
ASMSRC = asm.c code.c error.c expr.c file.c lex.c symbol.c syntax.c
ASMOBJ = $(ASMSRC:.c=.o)
LDFLAGS = -lm
CFLAGS = -g -D_DEBUG_
OBJS = ASMOBJS
DEPS = $(OBJS:.o:.dep)

TARGETS = mcm-asm



mcm-asm: $(ASMOBJ)
	$(CC) $(ASMOBJ) $(LDFLAGS) -o $@



dep: $(DEPS)
	cat $(DEPS) > deps

clean:
	rm -f *.o
	rm -f $(TARGETS)
	echo "" > deps


include deps
