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

// pids
int firstPID, secondPID;

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
* function name : endGame
* input : signum
* output : -
* explanation : this function is in charge of sending two SIGINT to the children
*               processes
*******************************************************************************/
void endGame(int signum) {
    if ((kill(firstPID, SIGINT)) < 0 ) {
        exitWithError("error signal");
    }

    if ((kill(secondPID, SIGINT)) < 0 ) {
        exitWithError("error signal");
    }

    sleep(3);
}

/*******************************************************************************
* function name : main
* input : argc, **argv
* output : 0
* explanation : main function
*******************************************************************************/
int main (int argc, char **argv) {
    int y;
    int fd[2];
    struct sigaction sigAlarm;
    sigset_t blocked;

    if (argc < 2) {
        exitWithError("No enough parameters");
    }

    // get running time
    y = atoi(argv[1]);

    sigemptyset(&blocked);
    // set handler for SIGALRM
    sigAlarm.sa_handler = endGame;
    sigAlarm.sa_mask = blocked;
    sigAlarm.sa_flags = 0;

    if ((sigaction(SIGALRM, &sigAlarm, NULL)) < 0) {
        exitWithError("error sigaction");
    }

    if ((pipe(fd)) < 0) {
        exitWithError("pipe error");
    };

    firstPID = fork();

    // first child - > ex2_inp
    if (firstPID == 0) {
        close(0);
        dup(fd[0]);

        close(fd[0]);
        close(fd[1]);

        execlp("./ex2_inp", "./ex2_inp", NULL);
    } else if (firstPID < 0) {
        exitWithError("fork error");
    }

    secondPID = fork();

    // second child - > ex2_upd
    if (secondPID == 0) {
        char pid[20];

        sprintf(pid, "%d", firstPID);
        close(1);
        dup(fd[1]);

        close(fd[0]);
        close(fd[1]);

        execlp("./ex2_upd", "./ex2_upd", pid ,NULL);
    } else if (secondPID < 0) {
        exitWithError("fork error");
    }

    // father
    if (secondPID > 0 && firstPID > 0) {
        close(fd[0]);
        close(fd[1]);
        alarm(y);

        pause();
    }

    return 0;
}

