all:
	mkdir bin -p
	gcc sources/version1.c -o bin/version1 -Wall -Werror -lm -fsanitize=address,leak
clean:
	rm bin/version1
	rmdir bin
