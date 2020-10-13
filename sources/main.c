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
	pipe(fdForP);
	if (fork() == 0) {
		dup2(fdForP[1], 1);
		close(fdForP[0]);
		close(fdForP[1]);
        execvp(cmd_A[0], cmd_A);
        _exit(1);
	}
	if (fork() == 0) {
		dup2(fdForP[0], 0);
		close(fdForP[0]);
		close(fdForP[1]);
		execvp(cmd_B[0], cmd_B);
        _exit(1);
	}
    close(fdForP[0]);
	close(fdForP[1]);
	wait(NULL);
	wait(NULL);
	return;
}

void pipeForN(char **list, int *iForP, int n) {
    int fdForP[n][2], pid;
    int input_fd = 0 , output_fd = 1;
    char cmd_io_array = get_list();
    char cmd_array = prepare_io(cmd_io_array, &input_fd, &output_fd);
    
    for (int i = 0; i < n ; i++) {
        pipe (fdForP[i + 1]);
        if (( pid = fork ()) == 0) {
            dup2 (fd[i - 1][0], 0);
            close (fd[i - 1][1]);
            close (fd[i - 1][0]);

            dup2 (fd[i][1], 1);
            close (fd[i][0]);
            close (fd[i][1]);

            execvp (cmd[i][0], cmd);
            return 1;
            execvp ( cmd_array [i][0] , cmd_array[i]);
        } else {
            /* close some fds */
            waitpid ( pid , NULL , 0);
        }
    }
    /* clear heap */
}

int flow(char **list) {
    int flag = 0;
    int fd = 0;
    int i = 0;
    int iForP = 0;

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
        }
        i++;
    }

    if (iForP != 0) {
        pipeForTwo(list, fd, iForP);
    } else {
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
    return 0;
}
