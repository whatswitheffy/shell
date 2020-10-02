%: %.c
	gcc -Wall -Werror -o $@ $@.c -fsanitize=address,leak
