//
// Created by guest on 5/20/17.
//

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <time.h>

#define MAX_LINE 4096
#define MAX_ITEMS 16
#define BOARD_SIZE 4
#define STDOUT 1

typedef enum {FALSE =0, TRUE} BOOLEAN;
typedef struct {
    int x;
    int y;
} Point;

// game board -- global variable.
int board[BOARD_SIZE][BOARD_SIZE];
// waiting time -- global variable
int waitTime;
// printer process id -- global variable
int pid;
// isFinished -- global variable
BOOLEAN isFinished = FALSE;

void exitWithError(char *msg) {
    perror(msg);
    exit(-1);
}

Point generateRandomPoint() {
    Point tmp;

    tmp.x = (rand() % BOARD_SIZE);
    tmp.y = (rand() % BOARD_SIZE);

    return tmp;
}

void generateStringFromBoard(char *buffer) {
    int i, j;

    char smallBuf[MAX_ITEMS] = {0};

    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {
            sprintf(smallBuf, "%d,", board[i][j]);
            strcat(buffer, smallBuf);
        }
    }

    // remove the extra ','
    buffer[strlen(buffer) - 1] = '\0';
}

void printAndSignal() {
    char buffer[MAX_LINE] = {0};

    // generate a string of the board
    generateStringFromBoard(buffer);

    // write it to STDOUT
    if ((write(STDOUT, buffer, strlen(buffer))) < 0) {
        exitWithError("Error writing to STDOUT");
    }


    // send a signal to the printer process
    if ((kill(pid, SIGUSR1)) < 0) {
        exitWithError("Error signaling");
    }
}

void initiateBoard() {
    int i, j;

    Point posArr[2];
    // reset board
    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = 0;
        }
    }

    // we want a number between 1 and 5
    waitTime = (rand() % 5) + 1;

    // generate a random point
    posArr[0] = generateRandomPoint();
    posArr[1] = generateRandomPoint();

    // we want two different points
    while ((posArr[0].x == posArr[1].x) && (posArr[0].y == posArr[1].y)) {
        posArr[1] = generateRandomPoint();
    }

    // assign tiles to the random points
    board[posArr[0].x][posArr[0].y] = 2;
    board[posArr[1].x][posArr[1].y] = 2;

    // print and signal SIGUSER1
    printAndSignal();
}

void waitTimeOver() {
    Point p = generateRandomPoint();

    // find an empty tile
    while (board[p.x][p.y] != 0) {
        p = generateRandomPoint();
    }

    // place 2 inside the empty tile
    board[p.x][p.y] = 2;

    // new wait time
    waitTime = (rand() % 5) + 1;

    // print and signal SIGUSER1
    printAndSignal();
}

int main(int argc, char **argv) {

    srand(time(NULL));
    initiateBoard();

}