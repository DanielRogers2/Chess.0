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

int main()
{
    //Load or generate tables
    if (!loadMoveTables())
    {
        //No tables available, generate them
        generateMoveTables();
    }

#ifdef DEBUG
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
#endif

    //Set up network code

    //Run the main program loop

    return (0);
}
