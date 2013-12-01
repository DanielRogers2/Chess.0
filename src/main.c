/*
 * main.c
 * 
 * Executes the program
 * 
 * @author Daniel Rogers
 * 
 */
#include <sys/time.h>

#include "board.h"
#include "pregame.h"
#include "brain.h"

#ifdef DEBUG
void playSampleGame(unsigned gamenum, uint8_t w_ply, uint8_t b_ply);
#endif

int main()
{
    //Load or generate tables
    if (!loadMoveTables())
    {
        //No tables available, generate them
        generateMoveTables();
    }

#ifdef DEBUG_TABLES
    printf("addr_LM: %p\n", &legal_moves);
    printf("move00: %d\n", legal_moves[0][0][0][0]);
#endif

    //Get a new board and initialize it
    chessboard current_state;
    initBoard(&current_state);

#ifdef DEBUG
    boardset newstates;
    newstates.count = 0;
    newstates.data = NULL;

    uint8_t exp = expandStates(&current_state, &newstates, true);

    printf("expanded: %u\n", exp);
    printf("%llx\n", newstates.data[0].all_w_pieces);

    chessboard res;
    //Test negamax
    puts("negamax:");

    //for white
    selectBestMove(true, &current_state, &res, 6);
    puts("did sBM for white");
    printf("piece: %d, move: %d\n", res.w_last_piece, res.w_last_move);
    printf("value: %d\n", evaluateState(&res, true));

    //for black, same initial state... should come up with the same move
    //  and value
    selectBestMove(false, &current_state, &res, 6);
    puts("did sBM for black");
    printf("piece: %d, move: %d\n", res.b_last_piece, res.b_last_move);
    printf("value: %d\n", evaluateState(&res, false));

    //Lower depth, for more info
    selectBestMove(false, &current_state, &res, 4);
    puts("did d4 sBM for black");
    printf("piece: %d, move: %d\n", res.b_last_piece, res.b_last_move);
    printf("value: %d\n", evaluateState(&res, false));

    //Game examples
    puts("testing game 4(w) vs 2(b)");
    playSampleGame(1, 4, 2);

    puts("testing game 5(w) vs 2(b)");
    playSampleGame(2, 5, 2);

    puts("testing game 2(w) vs 4(b)");
    playSampleGame(3, 2, 4);

    puts("testing game 2(w) vs 5(b)");
    playSampleGame(4, 2, 5);

    puts("testing game 5(w) vs 4(b)");
    playSampleGame(5, 5, 4);

    puts("testing game 4(w) vs 5(b)");
    playSampleGame(6, 4, 5);

    puts("testing game 5(w) vs 5(b)");
    playSampleGame(7, 5, 5);
#endif

    //Set up network code

    //Run the main program loop

    return (0);
}

#ifdef DEBUG
void playSampleGame(unsigned gamenum, uint8_t w_ply, uint8_t b_ply)
{
    //Timers
    clock_t tstart, tend;
    double tex;

    chessboard current_state;
    chessboard res;

    bool white_won, draw;
    draw = white_won = false;

    char mov_str[3];
    char piece[4];

    initBoard(&current_state);
    uint8_t counter = 0;
    while (true)
    {
        //white
        printf("white: turn %d\n", counter);
        tstart = clock();
        selectBestMove(true, &current_state, &res, w_ply);
        tend = clock();

        squareToString(res.w_last_move, mov_str);
        pieceToString(res.w_last_piece, res.w_codes[res.w_last_piece], piece);
        tex = (double) (tend - tstart) / CLOCKS_PER_SEC;

        printf("piece: %s, move: %s, value: %d in %f\n", piece, mov_str,
                evaluateState(&res, true), tex);
        ++counter;
        current_state = res;

        if (current_state.b_pieces[15] == CAPTURED)
        {
            white_won = true;
            break;
        }

        //black
        printf("black: turn %d\n", counter);
        tstart = clock();
        selectBestMove(false, &current_state, &res, b_ply);
        tend = clock();

        squareToString(res.b_last_move, mov_str);
        pieceToString(res.b_last_piece, res.b_codes[res.b_last_piece], piece);
        tex = (double) (tend - tstart) / CLOCKS_PER_SEC;

        printf("piece: %s, move: %s, value: %d in %f\n", piece, mov_str,
                evaluateState(&res, false), tex);
        ++counter;
        current_state = res;

        if (current_state.w_pieces[15] == CAPTURED)
        {
            white_won = false;
            break;
        }

        if (counter == 0)
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
}
#endif
