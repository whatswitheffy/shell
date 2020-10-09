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
    char *word = NULL;

    if (*end == '\n') {
        return NULL;
    }

    ch = getchar();

    if (ch == '\n') {
        *end = ch;
        return NULL;
    }
    while (i == 0 && (ch == ' ' || ch == '\t' )) { 
        ch = getchar();
        if (ch == '\n') {
            *end = ch;
            return NULL;
        }
    }
    while (ch != ' ' && ch != '\t' && ch != '\n') {
        word = (char *)realloc(word, (i + 1) * sizeof(char));
        word[i] = ch;
        i++;
        ch = getchar();
    }
    word = realloc(word, (i + 1) * sizeof(char));
    word[i] = '\0';
    *end = ch;
    return word;
}

char **getList() {
    char end = 0;
    char **list = NULL;
    int i = 0;
    while (end != '\n') {
        list = (char **)realloc(list, (i + 1) * sizeof(char*));
        list[i] = getWord(&end);
        i++;
    }
    list = (char **)realloc(list, (i + 1) * sizeof(char*));
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
    if (list[0] != NULL) {
        if ((strcmp(list[0], "exit") == 0) || (strcmp(list[0], "quit") == 0)) {
            return 1;
        } else {
            return 0;
        }
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
    printf("SUPER EVA'S TRMNAL >>");
    char **list = getList();
    while (!isExit(list)) {
        printf("\nSUPER EVA'S TRMNAL >>");
        flow(list);
        freeList(list);
        list = getList();
    }
    freeList(list);
    return 0;
}
