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

    printf("addr_LM: %p\n", &legal_moves);
    printf("move00: %d\n", legal_moves[0][0][0][0]);

    //Get a new board and initialize it
    chessboard current_state;
    initBoard(&current_state);

    chessboard * newstates = malloc(35 * sizeof(chessboard));
    uint8_t exp = expandStates(&current_state, newstates, true);

    printf("expanded: %u\n", exp);
    printf("%llx\n", newstates[0].all_w_pieces);
    free(newstates);

    //Set up network code

    //Run the main program loop

    return (0);
}
