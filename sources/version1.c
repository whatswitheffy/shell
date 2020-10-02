// Coopyright 2020 Nesterova Eva
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <err.h>
pid_t pid = 1;
char *get_word(char *end) {
    char ch;
    int pos = 0;
    char *word = NULL;
    if (*end == '\n') {
        return NULL;
    }
    do {
        ch = getchar();
        if (pos == 0 && ch == '\n') {
            return NULL;
        }
        while (pos == 0 && (ch == ' ' || ch == 't')) {
            ch = getchar();
            if (ch == '\n') {
                return NULL;
             }
        }
        word = realloc(word, (pos + 1) * sizeof(char));
        if (word == NULL) {
            err(1, NULL);
        }
        word[pos] = ch;
        pos++;
    } while (ch != ' ' && ch != '\t' && ch != '\n');
    word[pos - 1] = '\0';
    *end = ch;
    return word;
}

char **get_list(void) {
    char end = 0, **list = NULL, **ch = NULL;
    int i = 0;
    do {
	    ch = (char **)realloc(list, (i + 1) * sizeof(char *));
	    if (ch == NULL) {
    		err(1, NULL);
            }
	    list = ch;
	    list[i] = get_word(&end);
	    i++;
	} 
        while (list[i - 1] != NULL);
	return list;
}
void free_list(char **list) {
    for (int i = 0; list[i]; i++) {
        free(list[i]);
    }
    free(list);
}

void run(char **list) {
    int i = 0, fd_input = 0, fd_output = 1;
	while (list[i] != NULL) {
	    if (strcmp(list[i], "<") == 0) {
	        fd_input = open(list[i + 1], O_RDONLY);
	        if (fd_input < 0) {
		    perror("Open failed");
		    exit(1);
		}
		break;
		} else if (strcmp(list[i], ">") == 0) {
		    fd_output = open(list[i + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		    if (fd_output < 0) {
		        perror("Open failed");
			exit(1);
		    }
		    break;
                }
		i++;
        }
    list = realloc(list, (i + 1) * sizeof(char*));
    list[i] = NULL;
    return;
        }
        int is_exit(char **list) {
	if ((strcmp(list[0], "exit") == 0) || (strcmp(list[0], "quit") == 0)) {
		return 1;
	} else {
		return 0;
	}
}
int main(void) {
    char **list;
    while(1) {
	list = get_list();
	if (is_exit(list) == 1) {
	    free_list(list);
	    break;
	}
	run(list);
	free_list(list);
    }
	return 0;
}
