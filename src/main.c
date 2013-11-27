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

    //Get a new board and initialize it
    chessboard current_state;
    initBoard(&current_state);

    //Set up network code

    //Run the main program loop

    return (0);
}
