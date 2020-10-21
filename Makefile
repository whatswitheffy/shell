CFLAGS = -Wall -Werror -lm -g
main: src/main.c
	gcc src/$@.c -o bin/$@ $(CFLAGS)
