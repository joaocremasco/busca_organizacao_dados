CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -MMD -MP -Iinclude -Idata_structures/include
LDLIBS = -lm
ARGS ?=
PROGRAMA = trabalho
FONTES = $(wildcard src/*.c data_structures/src/*.c)
OBJETOS = $(FONTES:.c=.o)
DEPENDENCIAS = $(OBJETOS:.o=.d)
TESTE_TADS = tests/test_tads
FONTES_TADS = $(wildcard data_structures/src/*.c) src/collection.c src/item.c

all: $(PROGRAMA)

$(PROGRAMA): $(OBJETOS)
	$(CC) $(OBJETOS) $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(PROGRAMA)
	./$(PROGRAMA) $(ARGS)

$(TESTE_TADS): tests/test_tads.c $(FONTES_TADS)
	$(CC) $(CFLAGS) $^ -o $@

test: $(TESTE_TADS)
	./$(TESTE_TADS)

clean:
	rm -f $(OBJETOS) $(DEPENDENCIAS) $(PROGRAMA) $(TESTE_TADS) $(TESTE_TADS).d

-include $(DEPENDENCIAS)
.PHONY: all run test clean
