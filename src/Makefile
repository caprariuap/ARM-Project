CC      = gcc
CFLAGS  = -Wall -g -D_DEFAULT_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o lib/bitutils.o lib/data_types.o assemble_lib/assemble_instructions.o 

assemble_lib/assemble_instructions.o: assemble_lib/assemble_instructions.c lib/bitutils.h
	$(CC) $(CFLAGS)  assemble_lib/assemble_instructions.c -c -o assemble_lib/assemble_instructions.o

emulate: emulate.o lib/bitutils.o emulate_lib/decode.o emulate_lib/execute.o

emulate.o: emulate.c lib/bitutils.h
	$(CC) $(CFLAGS) emulate.c -c -o emulate.o

lib/bitutils.o: lib/bitutils.c lib/bitutils.h
	$(CC) $(CFLAGS) lib/bitutils.c -c -o lib/bitutils.o

lib/data_types.o: lib/data_types.c lib/data_types.h
	$(CC) $(CFLAGS) lib/data_types.c -c -o lib/data_types.o

emulate_lib/decode.o: emulate_lib/decode.c lib/bitutils.h
	$(CC) $(CFLAGS) emulate_lib/decode.c -c -o emulate_lib/decode.o

emulate_lib/execute.o: emulate_lib/execute.c lib/bitutils.h
	$(CC) $(CFLAGS) emulate_lib/execute.c -c -o emulate_lib/execute.o

clean:
	rm -f assemble assemble.o
	rm -f emulate emulate.o
	rm -f emulate_lib/*.o
	rm -f assemble_lib/*.o
	rm -f lib/*.o
