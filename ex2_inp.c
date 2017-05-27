/*
 * Student name : Or Zipori
 * Student : 302933833
 * Course Exercise Group : 03
 * Exercise Name : ex2
 */
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
#define WIN '7'
#define LOSE '8'
#define STDOUT 1

typedef enum {FALSE =0, TRUE} BOOLEAN;

// global variable for checking if we are done
BOOLEAN isFinished = FALSE;

/*******************************************************************************
* function name : exitWithError
* input : message
* output : -
* explanation : write to stderr the message and exit with code -1
*******************************************************************************/
void exitWithError(char *msg) {
    perror(msg);
    exit(-1);
}

/*******************************************************************************
* function name : parseString
* input : *data, *arr
* output : -
* explanation : parse the data string and create an int array and assign it to
*               arr
*******************************************************************************/
void parseString(char *data, int *arr) {
    char *token;
    int i = 0;

    token = strtok(data, ",");

    while (token != NULL) {
        arr[i] = atoi(token);

        token = strtok(NULL, ",");
        i++;
    }
}

/*******************************************************************************
* function name : handle_input
* input : signum
* output : -
* explanation : SIGUSR1 handler, in charge of printing to STDOUT the
*               deserialized board.
*******************************************************************************/
void handle_input(int signum) {
    char buffer[MAX_LINE] = {0}, outBuf[MAX_LINE] = {0}, tempBuf[100] = {0};
    int k = 0, i, j;
    int items[MAX_ITEMS];
    char *token, *mode;

    // read from STDIN
    if ((read(STDIN, &buffer, sizeof(buffer))) < 0) {
        exitWithError("error read");
    }

    // get the board serialization
    token  = strtok(buffer, "#");
    // get game mode
    mode = strtok(NULL, "#");

    // parse the data and assign it to an array
    parseString(token, items);

    // print the board
    for (i = 0; i < BOARD_SIZE; ++i) {
        strcat(outBuf,"|");
        for (j = 0; j < BOARD_SIZE; ++j) {
            k = (i * BOARD_SIZE) + j;
            if (items[k] == 0) {
                sprintf(tempBuf,"      |");
            } else {
                sprintf(tempBuf," %.4d |", items[k]);
            }
            strcat(outBuf, tempBuf);
        }
        strcat(outBuf,"\n");
    }

    strcat(outBuf,"\n");

    // print to the screen

    if ((write(STDOUT, outBuf, strlen(outBuf))) < 0) {
        exitWithError("error writing");
    }

    // check win or lose
    if (mode[0] == LOSE) {
        if ((write(STDOUT, "Game Over !", 12)) < 0) {
            exitWithError("error writing");
        }

        // use the alarm handler to end both child processes
        if ((kill(getppid(), SIGALRM)) < 0 ) {
            exitWithError("error signal");
        }
        return;
    }

    if (mode[0] == WIN) {
        if ((write(STDOUT, "Congratulations !", 18)) < 0) {
            exitWithError("error writing");
        }
        // use the alarm handler to end both child processes
        if ((kill(getppid(), SIGALRM)) < 0 ) {
            exitWithError("error signal");
        }
    }
}

/*******************************************************************************
* function name : handle_int
* input : signum
* output : -
* explanation : SIGINT handler, in charge of finish the game properly
*******************************************************************************/
void handle_int(int signum) {
    char buf[15];
    sprintf(buf,"\nBYE BYE\n");
    isFinished = TRUE;

    if ((write(STDOUT, buf, strlen(buf))) < 0) {
        exitWithError("error writing");
    }
}

/*******************************************************************************
* function name : main
* input : argc, **argv
* output : 0
* explanation : main function
*******************************************************************************/
int main (int argc, char **argv) {
    sigset_t blocked;
    struct sigaction sigusr1Handler;
    struct sigaction sigintHandler;
    char buf[100];

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

    if ((sigaction(SIGUSR1, &sigusr1Handler, NULL)) < 0) {
        exitWithError("error sigaction");
    }

    if ((sigaction(SIGINT, &sigintHandler, NULL)) < 0) {
        exitWithError("error sigaction");
    }

    while(!isFinished) {
        // pause until one of the two signals received
        pause();
    }

    return 0;
}

