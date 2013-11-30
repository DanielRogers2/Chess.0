/*
 * main.c
 * 
 * Executes the program
 * 
 * @author Daniel Rogers
 * 
 */

#include "board.h"
#include "pregame.h"
#include "brain.h"

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
    printf("piece: %d, move: %d\n", res.last_piece, res.last_move);
    printf("value: %d\n", evaluateState(&res, true));
    //for black, same initial state... should come up with the same move
    //  and value
    selectBestMove(false, &current_state, &res, 6);
    puts("did sBM for black");
    printf("piece: %d, move: %d\n", res.last_piece, res.last_move);
    printf("value: %d\n", evaluateState(&res, false));
    //Lower depth, for more info
    selectBestMove(false, &current_state, &res, 4);
    puts("did d4 sBM for black");
    printf("piece: %d, move: %d\n", res.last_piece, res.last_move);
    printf("value: %d\n", evaluateState(&res, false));

    puts("testing game 4(w) vs 2(b)");
    uint16_t counter = 0;
    while (current_state.w_locations[15] && current_state.b_locations[15])
    {
        //white, to depth 4
        selectBestMove(true, &current_state, &res, 4);
        current_state = res;
        //black, to depth 2
        selectBestMove(false, &current_state, &res, 2);
        if ((counter % 10) == 0)
        {
            printf("turn %d\n", counter);
        }
        ++counter;
        if (counter == 1000)
        {
            puts("stalemate maybe...");
        }
    }

    if (current_state.w_locations[15])
    {
        puts("winner was 4-ply white! yay it.. works?");
    }
    else if (current_state.b_locations[15])
    {
        puts("2-ply black won.. back to drawing board");
    }
    else
    {
        puts("uh-oh");
    }

    initBoard(&current_state);

    puts("testing game 2(w) vs 4(b)");
    counter = 0;
    while (current_state.w_locations[15] && current_state.b_locations[15])
    {
        //white, to depth 4
        selectBestMove(true, &current_state, &res, 2);
        current_state = res;
        //black, to depth 2
        selectBestMove(false, &current_state, &res, 4);
        if ((counter % 10) == 0)
        {
            printf("turn %d\n", counter);
        }
        ++counter;
        if (counter == 1000)
        {
            puts("stalemate maybe...");
        }
    }

    if (current_state.w_locations[15])
    {
        puts("2-ply white won.. back to drawing board");
    }
    else if (current_state.b_locations[15])
    {
        puts("winner was 4-ply black! yay it.. works?");
    }
    else
    {
        puts("uh-oh");
    }
#endif

    //Set up network code

    //Run the main program loop

    return (0);
}
