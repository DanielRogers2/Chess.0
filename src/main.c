/*
 * main.c
 * 
 * Executes the program
 * 
 * @author Js
 * 
 */
#include <sys/time.h>

#include "board.h"
#include "pregame.h"
#include "brain.h"

#define INITIAL_DEPTH 7

#ifdef PLAY_SELF
void playSampleGame(uint8_t w_ply, uint8_t b_ply);
#endif

void getPlayerMove(char move[7]);

int main(int argc, const char * argv[])
{
    //Load or generate tables
    if (!loadMoveTables())
    {
        //No tables available, generate them
        generateMoveTables();
    }

    //Get a new board and initialize it
    chessboard current_state;
    chessboard next_state;

    initBoard(&current_state);

#ifdef PLAY_SELF
    if (argc < 3)
    {
        puts("Usage: w_ply, b_ply");
        return (0);
    }
    playSampleGame((uint8_t) atoi(argv[1]), (uint8_t) atoi(argv[2]));
    return 0;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#endif

    if (argc < 2)
    {
        puts("Usage: <w|b> [<initial_depth>]");
        return (0);
    }

    uint8_t depth = (argc >= 3) ? (uint8_t) atoi(argv[2]) : INITIAL_DEPTH;

    bool self_white = (argv[1][0] == 'w') ? true : false;

    //The play they made/we made
    char move[7];

    if (self_white)
        goto WHITE_START;

    //Game loop
    while (true)
    {
        //Get their move
        getPlayerMove(move);

        printf("received move: %s\n", move);

        //Parse the move
        parseMoveString(move, !self_white, &current_state);
        printBoard(&current_state);

WHITE_START:
        //Make move
        selectBestMove(self_white, &current_state, &next_state, depth);

        //Extract the move
        getMoveString(&next_state, &current_state, self_white, move);

        printf("CPU Move: %s\n", move);
        printBoard(&current_state);

        //Update current state
        current_state = next_state;
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
    return (0);
#pragma clang diagnostic pop

#ifdef PLAY_SELF
#pragma clang diagnostic pop
#endif
}

void getPlayerMove(char move[7])
{
    puts("enter a move, e.g. PA2A3:");
    fgets(move, 7, stdin);
    if(move[6] == '\n')
    {
        move[6] = '\0';
    }

    if(move[5] == '\n')
    {
        move[5] = '\0';
    }
}

#ifdef PLAY_SELF
void playSampleGame(uint8_t w_ply, uint8_t b_ply)
{
    //Timers
    clock_t tstart, tend;
    double tex;

    //Record a play string to allow animation if desired
    char plays[256][6] =
    {
        {   0}};

    chessboard current_state;
    chessboard res;

    bool white_won, draw;
    draw = white_won = false;

    initBoard(&current_state);
    uint16_t counter = 0;
    int waiter;

    while (true)
    {
        //white
        printf("white: turn %d\n", counter);
        tstart = clock();
        selectBestMove(true, &current_state, &res, w_ply);
        tend = clock();

        getMoveString(&res, &current_state, true, plays[counter]);
        tex = (double) (tend - tstart) / CLOCKS_PER_SEC;

        printf("move: %s value: %d, time: %f\n", plays[counter],
                evaluateState(&res, true), tex);
        ++counter;
        current_state = res;

        if (current_state.b_piece_posns[15] == CAPTURED)
        {
            white_won = true;
            break;
        }

        printBoard(&current_state);
        waiter = getchar();

        //black
        printf("black: turn %d\n", counter);
        tstart = clock();
        selectBestMove(false, &current_state, &res, b_ply);
        tend = clock();

        getMoveString(&res, &current_state, false, plays[counter]);
        tex = (double) (tend - tstart) / CLOCKS_PER_SEC;

        printf("move: %s value: %d, time: %f\n", plays[counter],
                evaluateState(&res, false), tex);
        ++counter;
        current_state = res;

        if (current_state.w_piece_posns[15] == CAPTURED)
        {
            white_won = false;
            break;
        }

        if (counter == 500)
        {
            puts("stalemate maybe...");
            draw = true;
            break;
        }

        printBoard(&current_state);
        waiter = getchar();
    }

    if (white_won)
    {
        puts("White wins!");
    }
    else if (!draw)
    {
        puts("Black wins!");
    }
    else
    {
        puts("Draw.. maybe?");
    }

    printf("board state\n");
    printBoard(&current_state);

    counter = counter - 1;

    puts("move history: \n[ ");
    for (uint8_t i = 0; i < counter; ++i)
    {
        printf("\"%s\",", plays[i]);
    }
    printf("\"%s\" ]\n", plays[counter]);
}
#endif
