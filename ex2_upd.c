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
* function name : checkWin
* input : -
* output : true if win else false
* explanation : checks if the player has won
*******************************************************************************/
BOOLEAN checkWin() {
    int i, j;
    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == 2048) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

/*******************************************************************************
* function name : checkLose
* input : -
* output : true if lose else false
* explanation : checks if the player has lost
*******************************************************************************/
BOOLEAN checkLose() {
    int i, j;
    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == 0) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*******************************************************************************
* function name : setAlarm
* input : -
* output : -
* explanation : set an alarm with random value
*******************************************************************************/
void setAlarm() {
    // we want a number between 1 and 5
    waitTime = (rand() % 5) + 1;
    alarm(waitTime);
}

/*******************************************************************************
* function name : generateRandomPoint
* input : -
* output : random point
* explanation : generate a random point
*******************************************************************************/
Point generateRandomPoint() {
    Point tmp;

    tmp.x = (rand() % BOARD_SIZE);
    tmp.y = (rand() % BOARD_SIZE);

    return tmp;
}

/*******************************************************************************
* function name : getGameMode
* input : *buffer
* output : -
* explanation : assign a game mode inside the buffer. 7 - win, 8 - lose,
*               6 - continue.
*******************************************************************************/
void getGameMode(char *buffer) {
    int mode;

    if (checkLose()) {
        strcpy(buffer,"#8");
    } else if (checkWin()) {
        strcpy(buffer,"#7");
    } else {
        strcpy(buffer,"#6");
    }
}

/*******************************************************************************
* function name : generateStringFromBoard
* input : *buffer
* output : -
* explanation : generate and assign a board serialization inside the buffer
*******************************************************************************/
void generateStringFromBoard(char *buffer) {
    int i, j;
    char mode[3];

    char smallBuf[MAX_ITEMS] = {0};

    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {
            sprintf(smallBuf, "%d,", board[i][j]);
            strcat(buffer, smallBuf);
        }
    }

    getGameMode(mode);

    // remove the extra ','
    buffer[strlen(buffer) - 1] = '\0';

    // add game mode serialization
    strcat(buffer, mode);
}

/*******************************************************************************
* function name : printAndSignal
* input : -
* output : -
* explanation : write to stdout and signal ex2_inp to print
*******************************************************************************/
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

/*******************************************************************************
* function name : initiateBoard
* input : -
* output : -
* explanation : initiate a new board and set two random tiles with 2
*******************************************************************************/
void initiateBoard() {
    int i, j;

    Point posArr[2];
    // reset board
    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = 0;
        }
    }

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

    setAlarm();

    // print and signal SIGUSER1
    printAndSignal();
}

/*******************************************************************************
* function name : waitTimeOver
* input : sig
* output : -
* explanation : SIGALRM handler, generates a point and asign the tile 2
*******************************************************************************/
void waitTimeOver(int sig) {
    Point p = generateRandomPoint();

    // find an empty tile
    while (board[p.x][p.y] != 0) {
        p = generateRandomPoint();
    }

    // place 2 inside the empty tile
    board[p.x][p.y] = 2;

    // print and signal SIGUSER1
    printAndSignal();

    // new wait time
    setAlarm();
}

/*******************************************************************************
* function name : slideUp
* input : -
* output : -
* explanation : handles moving all the columns up
*******************************************************************************/
void slideUp() {
    int i, k, j;
    // determines where to stop and not to merge because tile was merged
    int stop;

    // start with columns
    for (j = 0; j < BOARD_SIZE; ++j) {
        stop = -1;
        for (i = 1; i < BOARD_SIZE; ++i) {
            // if it's an empty tile we continue
            if (board[i][j] == 0) {
                continue;
            }

            for (k = i - 1; k >= 0; --k) {
                // if we can merge tiles
                if ((board[k + 1][j] == board[k][j]) && (stop < k)) {
                    // merge
                    board[k][j] *=2;
                    // set a stop
                    stop = k;
                    // empty the tile
                    board[k + 1][j] = 0;
                } else {
                    if (board[k][j] == 0) {
                        board[k][j] = board[k + 1][j];
                        board[k + 1][j] = 0;
                    } else {
                        // can't move anymore
                        break;
                    }
                }
            }
        }
    }
}

/*******************************************************************************
* function name : slideDown
* input : -
* output : -
* explanation : handles moving all the columns down
*******************************************************************************/
void slideDown() {
    int i, k, j;
    // determines where to stop and not to merge because tile was merged
    int stop;

    // start with columns
    for (j = 0; j < BOARD_SIZE; ++j) {
        stop = BOARD_SIZE;
        for (i = (BOARD_SIZE - 2); i >= 0; --i) {
            // if it's an empty tile we continue
            if (board[i][j] == 0) {
                continue;
            }

            for (k = i + 1; k < BOARD_SIZE; ++k) {
                // if we can merge tiles
                if ((board[k - 1][j] == board[k][j]) && (stop > k)) {
                    // merge
                    board[k][j] *=2;
                    // set a stop
                    stop = k;
                    // empty the tile
                    board[k - 1][j] = 0;
                } else {
                    if (board[k][j] == 0) {
                        board[k][j] = board[k - 1][j];
                        board[k - 1][j] = 0;
                    } else {
                        // can't move anymore
                        break;
                    }
                }
            }
        }
    }
}

/*******************************************************************************
* function name : slideRight
* input : -
* output : -
* explanation : handles moving all the rows right
*******************************************************************************/
void slideRight() {
    int i, k, j;
    // determines where to stop and not to merge because tile was merged
    int stop;

    // start with rows
    for (i = 0; i < BOARD_SIZE; ++i) {
        stop = BOARD_SIZE;
        for (j = (BOARD_SIZE - 2); j >= 0; --j) {
            // if it's an empty tile we continue
            if (board[i][j] == 0) {
                continue;
            }

            for (k = j + 1; k < BOARD_SIZE; ++k) {
                // if we can merge tiles
                if ((board[i][k - 1] == board[i][k]) && (stop > k)) {
                    // merge
                    board[i][k] *=2;
                    // set a stop
                    stop = k;
                    // empty the tile
                    board[i][k - 1] = 0;
                } else {
                    if (board[i][k] == 0) {
                        board[i][k] = board[i][k - 1];
                        board[i][k - 1] = 0;
                    } else {
                        // can't move anymore
                        break;
                    }
                }
            }
        }
    }
}

/*******************************************************************************
* function name : slideLeft
* input : -
* output : -
* explanation : handles moving all the rows left
*******************************************************************************/
void slideLeft () {
    int i, k, j;
    // determines where to stop and not to merge because tile was merged
    int stop;

    // start with rows
    for (i = 0; i < BOARD_SIZE; ++i) {
        stop = -1;
        for (j = 1; j < BOARD_SIZE; ++j) {
            // if it's an empty tile we continue
            if (board[i][j] == 0) {
                continue;
            }

            for (k = j - 1; k >= 0; --k) {
                // if we can merge tiles
                if ((board[i][k + 1] == board[i][k]) && (stop < k)) {
                    // merge
                    board[i][k] *=2;
                    // set a stop
                    stop = k;
                    // empty the tile
                    board[i][k + 1] = 0;
                } else {
                    if (board[i][k] == 0) {
                        board[i][k] = board[i][k + 1];
                        board[i][k + 1] = 0;
                    } else {
                        // can't move anymore
                        break;
                    }
                }
            }
        }
    }
}

/*******************************************************************************
* function name : runGameLogic
* input : move
* output : -
* explanation : invoking the correct slide function according to move and set
*               a new alarm
*******************************************************************************/
void runGameLogic(char move) {
    switch (move) {
        case 'W':
        case 'w':
            slideUp();
            setAlarm();
            break;
        case 'A':
        case 'a':
            slideLeft();
            setAlarm();
            break;
        case 'D':
        case 'd':
            slideRight();
            setAlarm();
            break;
        case 'X':
        case 'x':
            slideDown();
            setAlarm();
            break;
        case 'S':
        case 's':
            initiateBoard();
            break;
        default: // prevention of wrong chars
            return;
    }

    printAndSignal();
}

/*******************************************************************************
* function name : main
* input : argc, **argv
* output : 0
* explanation : main function
*******************************************************************************/
int main(int argc, char **argv) {
    sigset_t blocked;
    struct sigaction sigAlarm;
    char move;

    sigemptyset(&blocked);
    // set handler for SIGALRM
    sigAlarm.sa_handler = waitTimeOver;
    sigAlarm.sa_mask = blocked;
    sigAlarm.sa_flags = 0;

    if ((sigaction(SIGALRM, &sigAlarm, NULL)) < 0) {
        exitWithError("sigaction error");
    }

    pid = atoi(argv[1]);

    // for randomize
    srand(time(NULL));
    initiateBoard();

    while (!isFinished) {
        system("stty cbreak -echo");
        move = getchar();
        system("stty cooked echo");

        runGameLogic(move);
    }

    return 0;
}