/*
 * brain.c
 * 
 * Contains the implementations of the functions in brain.h
 * 
 * @author Daniel Rogers
 * 
 */

#include <stdbool.h>
#include "brain.h"

/*
 * Does a search for and returns the best move for the board
 *
 * @owner Daniel Rogers
 *
 * @param self_white true if we are white
 * @param initial A pointer to the initial board state to use
 * @param result A pointer that will be filled with the new board state based
 *               on the function's selected best move. last_piece and last_move
 *               will be set to the value of the piece & location to move it to
 */
void selectBestMove(bool self_white, chessboard * const initial,
        chessboard * result)
{
    //TODO implement this
}

/*
 * Performs a standard negamax search
 *
 * The caller is responsible for doing the first expansion & call to negamax
 *  for each root node, as well as determining which root node to use.
 *
 * @owner Daniel Rogers
 *
 * @param state A pointer to the start state for the layer
 * @param white If the current layer of the search is from white or black's
 *          perspective
 * @param expansionStore An array of chessboard * to use for storing expanded
 *                       states. These may be realloc'd durring execution of
 *                       the search.
 *                       !!! It is assumed to contain at least as many
 *                       pointers as the maximum depth of the search. Supplying
 *                       less will result in out of bounds memory access. !!!
 * @param depth The depth to traverse to
 *
 * @return The best score resulting from the negamax search
 */
int negamax(chessboard * const state, bool white, chessboard ** expansionStore,
        uint8_t depth)
{
    if (depth)
    {
        //Best value seen
        int best = INT_MIN;
        //Currently seen value
        int cur;
        //Storage of expanded states
        chessboard * storage = expansionStore[depth - 1];

        //Do expansion, store result
        uint8_t states = expandStates(state, storage, white);

        //recurse negamax for each state expanded
        for (uint8_t i = 0; i < states; ++i)
        {
            cur = -negamax(&storage[i], !white, expansionStore, depth - 1);
            if (cur > best)
            {
                best = cur;
            }
        }

        //Return the best value
        return (best);
    }
    else
    {
        //Return value of state
        return (evaluateState(state, white));
    }
}
