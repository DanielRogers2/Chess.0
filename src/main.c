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
#include "net.h"

#define INITIAL_DEPTH 7
#define INITIAL_TIME 900

#ifdef DEBUG
void playSampleGame(unsigned gamenum, uint8_t w_ply, uint8_t b_ply);
#endif

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

    //Need to know side, gameid, teamnumber, teamsecret
    if (argc < 5)
    {
        puts(
                "Usage: <w|b> <teamnumber> <teamsecret> <gameid> [<initial_depth>]");
        return (0);
    }

    bool self_white = (argv[1][0] == 'w') ? true : false;
    int teamnumber = atoi(argv[2]);
    const char * teamsecret = argv[3];
    int gameid = atoi(argv[4]);

    uint8_t depth = (argc >= 6) ? (uint8_t) atoi(argv[5]) : INITIAL_DEPTH;

    //The play they made/we made
    char move[7];

    double tlimit = INITIAL_TIME;

    if (self_white)
        goto WHITE_START;

    //Game loop
    while (true)
    {
        puts("\ngetting status");
        //Get their move
        getStatus(move, &tlimit, gameid, teamnumber, teamsecret);

        printf("received move: %s\n", move);

        //Parse the move
        parseMoveString(move, !self_white, &current_state);

        printf("making move, with tleft: %f\n", tlimit);

        WHITE_START:
        //Make move
        selectBestMove(self_white, &current_state, &next_state, depth, tlimit);

#ifdef DEBUG
        puts("result board:");
        printBoard(&next_state);
        puts("getting move string");
#endif
        //Extract the move
        getMoveString(&next_state, &current_state, self_white, move);

        printf("sending move: %s\n", move);

        //Submit move to server
        pushMove(gameid, teamnumber, teamsecret, move);

        //Update current state
        current_state = next_state;
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
    return (0);
#pragma clang diagnostic pop
}

#ifdef DEBUG
void playSampleGame(unsigned gamenum, uint8_t w_ply, uint8_t b_ply)
{
    //Timers
    clock_t tstart, tend;
    double tex;

    //15 mins each
    double twhite = 900;
    double tblack = 900;

    //Record a play string to allow animation if desired
    char plays[256][7] =
    {
        {   0}};

    chessboard current_state;
    chessboard res;

    bool white_won, draw;
    draw = white_won = false;

    initBoard(&current_state);
    uint16_t counter = 0;

    while (true)
    {
        //white
        printf("white: turn %d\n", counter);
        tstart = clock();
        selectBestMove(true, &current_state, &res, w_ply, twhite);
        tend = clock();

        getMoveString(&res, &current_state, true, plays[counter]);
        tex = (double) (tend - tstart) / CLOCKS_PER_SEC;
        //5 seconds back every move
        twhite -= (tex - 5);

        printf("move: %s value: %d, time: %f\n", plays[counter],
                evaluateState(&res, true), twhite);
        ++counter;
        current_state = res;

        if (current_state.b_pieces[15] == CAPTURED || tblack <= 0)
        {
            white_won = true;
            break;
        }

        //black
        printf("black: turn %d\n", counter);
        tstart = clock();
        selectBestMove(false, &current_state, &res, b_ply, tblack);
        tend = clock();

        getMoveString(&res, &current_state, false, plays[counter]);
        tex = (double) (tend - tstart) / CLOCKS_PER_SEC;
        //5 seconds back every move
        tblack -= (tex - 5);

        printf("move: %s value: %d, time: %f\n", plays[counter],
                evaluateState(&res, false), tblack);
        ++counter;
        current_state = res;

        if (current_state.w_pieces[15] == CAPTURED || twhite <= 0)
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

    printf("board state %u\n", gamenum);
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
