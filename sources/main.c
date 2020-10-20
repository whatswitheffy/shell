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

void pipeForTwo(char **list, int iForP) {
    int fdForP[2];
    int i, k, j, p;
    char **cmd_A, **cmd_B;
    cmd_A = (char **)malloc((iForP + 1) * sizeof(char*));
    for (i = 0; i < iForP; i++) {
        cmd_A[i] = list[i];
    }
    cmd_A[i] = NULL;
    i = iForP + 1;
    while (list[i] != NULL) {
        i++;
    }
    cmd_B = (char **)malloc((i - iForP) * sizeof(char*));
    k = 0;
    for (j = iForP + 1; j < i;j++) {
        cmd_B[k] = list[j];
        k++; 
    }
    cmd_B[k] = NULL;
    list[iForP] = NULL;
    iForP = 0;
    while (list[i] != NULL) {
        if (strcmp(list[i], "|") == 0) {
            iForP = i;
        }
        i++;
    }
}

char ***getCmdArr(char **list, int n) {
    int j = 0;
    int pipePosition = 0;
    int lstPipePos = 0;
    int i = 0;
    //ls -l | grep .txt | sort 
    char ***cmdArr = NULL;
    cmdArr = (char ***)malloc((n + 1) * sizeof(char **));
    while (list[i] != NULL) {
        if(strcmp(list[i], "|") == 0) {
            cmdArr[j] = (char **)malloc((i - lstPipePos + 1) * sizeof(char *));
            pipePosition = i;
            for(int i = 0; i < pipePosition - lstPipePos; i++) { 
                cmdArr[j][i] = list[i + lstPipePos];
            }
            cmdArr[j][pipePosition - lstPipePos] = NULL;
            lstPipePos = pipePosition + 1;
            j++; 
        } 
        i++; 
    }   
    cmdArr[j] = (char **)malloc((i - lstPipePos + 1) * sizeof(char *));
    pipePosition = i;
    for(int i = 0; i < pipePosition - lstPipePos; i++) { 
        cmdArr[j][i] = list[i + lstPipePos];
    }
    cmdArr[j][pipePosition - lstPipePos] = NULL;
    return cmdArr;
}

void runManyCommands(char **list, int *iForP, int n) {
    int fdForP[n][2], pid;
    char ***cmd_array = getCmdArr(list, n);
    for (int i = 0; i < n + 1; i++) {
        if (i != n) {
            pipe(fdForP[i]);
        }
        if ((pid = fork ()) == 0) {
            if (i != 0) {
                dup2(fdForP[i - 1][0], 0);
                close(fdForP[i - 1][0]);
                close(fdForP[i - 1][1]);
            }

            if (i != n) {
                dup2(fdForP[i][1], 1);
            }
            close(fdForP[i][0]);
            close(fdForP[i][1]);
            execvp(cmd_array[i][0], cmd_array[i]);
        } else {
            if (i != 0) {
                close(fdForP[i - 1][0]);
            }
            close(fdForP[i][1]);
            waitpid(pid, NULL, 0);
        }
    }
    
}

int *background_pids = NULL;
int count_background = 0; 

int flow(char **list) {
    int flag = 0, fd = 0, i = 0, iForP = 0, n = 0;
    int pid;
    int background = 0;
    char * getenv (const char * name ); 
    int setenv (const char * name, const char * value, int overwrite);
    const char *home = getenv("HOME");
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
        } else if (strcmp(list[i], "|") == 0) {
            iForP = i;
            n++;
        } else if (strcmp(list[i], "&") == 0) {
            background = 1;
            free(list[i]);
            list[i] = NULL;
            break;
        } else if (strcmp(list[0] , "cd") == 0) {
            if (list[1] == NULL || strcmp (list[1] , "~" ) == 0) {
                chdir (home);
                return 0;
            } else {
                chdir (list[1]);
                return 0;
            }
            }
        i++;
    }
    if (iForP != 0) {
        runManyCommands(list, &iForP, n);
    } else {
        if ((pid = fork()) > 0) {
            if (background == 0) {
                wait(NULL);
            } else {
                count_background++;
                background_pids = realloc(background_pids, background * sizeof(int));
                background_pids[count_background - 1] = pid;
            }
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
    }
    if (fd) {
        close(fd);
    }
    fd = 0;
}

int main(void) {    
    printf("SUPER EVA'S TRMNAL >>");
    char **list = getList();
    while (!isExit(list)) {
        printf("SUPER EVA'S TRMNAL >>");
        flow(list);
        freeList(list);
        list = getList();
    }
    freeList(list);
    for (int i = 0; i < count_background; i++) {
        waitpid(background_pids[i], NULL, 0);
    }
    return 0;
}
