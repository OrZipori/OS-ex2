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

int* parseString(char *data) {
    char *token;
    int i = 0;
    int *tempArr = (int *) malloc(MAX_ITEMS * sizeof(int));
    if (NULL == tempArr)
        exitWithError("Malloc error");

    token = strtok(data, ",");

    while (token != NULL) {
        tempArr[i] = atoi(token);

        token = strtok(NULL, ",");
        i++;
    }

    return tempArr;
}

void handle_input(int signum) {
    char buffer[MAX_LINE];
    int *items;
    int k = 0, i, j;

    // read from STDIN
    if ((read(STDIN, &buffer, sizeof(buffer))) < 0) {
        exitWithError("error read");
    }

    // parse the data and assign it to an array
    items = parseString(buffer);

    // print the board
    for (i = 0; i < BOARD_SIZE; ++i) {
        printf("|");
        for (j = 0; j < BOARD_SIZE; ++j) {
            k = (i * BOARD_SIZE) + j;
            if (items[k] == 0) {
                printf("      |");
            } else {
                printf(" %.4d |", items[k]);
            }
        }
        printf("\n");
    }

    free(items);
}

void handle_int(int signum) {
    printf("BYE BYE\n");
    isFinished = TRUE;
}

int main (int argc, char **argv) {
    sigset_t blocked;
    struct sigaction sigusr1Handler;
    struct sigaction sigintHandler;
    char buf[100] = "2,4,0,0,2,2,0,16,0,0,4,0,16,0,16,0";


    // block all signals except SIGUSR1 and SIGINT
    sigemptyset(&blocked);
    sigfillset(&blocked);
    sigdelset(&blocked, SIGUSR1);
    sigdelset(&blocked, SIGINT);

    // set handler for sigusr1
    sigusr1Handler.sa_handler = handle_input;
    sigusr1Handler.sa_mask = blocked;
    sigusr1Handler.sa_flags = 0;

    // set handler for sigint
    sigintHandler.sa_handler = handle_int;
    sigintHandler.sa_mask = blocked;
    sigintHandler.sa_flags = 0;

    sigaction(SIGUSR1, &sigusr1Handler, NULL);
    sigaction(SIGINT, &sigintHandler, NULL);


    while(!isFinished) {
        // pause until one of the two signals received

        pause();
    }

    return 0;
}

