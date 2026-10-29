CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -MMD -MP -Iinclude -Idata_structures/include
LDLIBS = -lm
ARGS ?=
PROGRAMA = trabalho
FONTES = $(wildcard src/*.c data_structures/src/*.c)
OBJETOS = $(FONTES:.c=.o)
DEPENDENCIAS = $(OBJETOS:.o=.d)

all: $(PROGRAMA)

$(PROGRAMA): $(OBJETOS)
	$(CC) $(OBJETOS) $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(PROGRAMA)
	./$(PROGRAMA) $(ARGS)

clean:
	rm -f $(OBJETOS) $(DEPENDENCIAS) $(PROGRAMA)

-include $(DEPENDENCIAS)
.PHONY: all run clean
