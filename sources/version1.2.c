#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <err.h>

char *getWord(char *end) {
    int i = 0;
    char ch;
    ch = getchar();
    char *word = NULL;

    if (*end == '\n') {
        return NULL;
    }
    if (ch == '\n') {
        return NULL;
    }
    while (i == 0 && (ch == ' ' || ch == '\t' )) { 
        ch = getchar();
        if (ch == '\n') {
            return NULL;
        }
    }
    while (ch != ' ' && ch != '\t' && ch != '\n') {
        word = realloc(word, (i + 1) * sizeof(char));
        word[i] = ch;
        i++;
        ch = getchar();
    }
    word[i] = '\0';
    *end = ch;
    return word;
}

char **getList() {
    char end;
    char **list = NULL;
    char *ch = NULL;
    int i = 0;
    while (end != '\n') {
        ch = getWord(&end);
        i++;
        list = (char **)realloc(list, (i + 1) * sizeof(char*));
        list[i - 1] = ch;
    }
    list[i] = NULL;
    return list; 
}

void freeList(char **list) {
    for (int i = 0; list[i]; i++) {
        free(list[i]);
    }
    free(list);
}


int isExit(char **list) {
    if ((strcmp(list[0], "exit") == 0) || (strcmp(list[0], "quit") == 0)) {
        return 1;
    } else {
        return 0;
    }
}

int flow(char **list) {
    int flag = 0;
    int fd = 0;
    int i = 0;
    while (list[i] != NULL) {
        if (strcmp(list[i], "<") == 0) {
            fd = open(list[i + 1], O_RDONLY);
            flag = 0;
            if (fd < 0) {
                perror("Open failed");
                exit(1);
            }
                free(list[i + 1]);
                list[i] = NULL;
            break;
        } else if (strcmp(list[i], ">") == 0) {
            fd = open(list[i + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            flag = 1;       
            if (fd < 0) {
                perror("Open failed");
                exit(1);
            }
            free(list[i + 1]);
            list[i] = NULL;
            break;
        }
        i++;
    }
    if (fork() > 0) {
        wait(NULL);
    } else {
        int tmp;
        if (fd) {
            dup2(fd, flag);
        }
        if (execvp(list[0], list) < 0) {
            perror("exec failed");
            return 1;
        }
        return 1;
    }
    if (fd) 
        close(fd);
    fd = 0;
}

int main(void) {
    char **list = getList();
    while (!isExit(list)) {
        flow(list);
        freeList(list);
        list = getList();
    }
    freeList(list);
    return 0;
}