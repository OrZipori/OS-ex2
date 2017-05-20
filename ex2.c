//
// Created by guest on 5/19/17.
//
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>

#define MAX_LINE 4096
#define MAX_ITEMS 16
#define BOARD_SIZE 4
#define STDIN 0

typedef enum {FALSE =0, TRUE} BOOLEAN;

// global variable for checking if we are done
BOOLEAN isFinished = FALSE;

void exitWithError(char *msg) {
    perror(msg);
    exit(-1);
}


int main (int argc, char **argv) {
    int firstPID, secondPID;
    int fd[2];

    pipe(fd);

    close(1);
    dup(fd[1]);

    firstPID = fork();


    if (firstPID == 0) {
        close(0);
        dup(fd[0]);

        puts("1");

        execlp("./ex2_inp1", "./ex2_in1p", NULL);
        printf("2");
    } else {
        char buf[100] = "2,4,0,0,2,2,0,16,0,0,4,0,16,0,16,0";
        write(1, buf, strlen(buf));

        kill(firstPID, SIGUSR1);
        sleep(5);
    }

    return 0;
}

