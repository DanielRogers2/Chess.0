/*
 * brain.h
 * 
 * Function definitions for running search
 * 
 * @author Daniel Rogers
 * 
 */

#ifndef BRAIN_H_
#define BRAIN_H_

#include <inttypes.h>

#include "common_defs.h"
#include "board.h"

//Safty margins for depth
//4mins, switch to depth medium
#define MED_DEPTH_CUTOFF 300
#define MEDIUM_DEPTH 6
//45s, switch to depth small
#define SMALL_DEPTH_CUTOFF 60
#define SMALLEST_DEPTH 5

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
 * @param tlimit The amount of time left on the clock in seconds
 */
void selectBestMove(bool self_white, chessboard * restrict const initial,
        chessboard * restrict result, uint8_t depth, double tlimit);

/*
 * Performs a standard negamax search
 *
 * The callee is responsible for doing the first expansion & call to negamax
 *  for each root node, as well as determining which root node to use.
 *
 * @owner Daniel Rogers
 *
 * @param state A pointer to the start state for the layer
 * @param white If the current layer of the search is from white or black's
 *          perspective
 * @param expansionStore An array of chessboard * to use for storing expanded
 *                       states. These may be realloc'd during execution of
 *                       the search.
 *                       !!! It is assumed to contain at least as many
 *                       pointers as the maximum depth of the search. Supplying
 *                       less will result in out of bounds memory access. !!!
 * @param alpha Best value seen, should initially be INT_MIN + 1, not INT_MIN
 * @param beta The cutoff
 * @param depth The depth to traverse to
 *
 * @return The best score resulting from the negamax search
 */
int negamax(chessboard * const state, bool white, boardset * expansionStore,
        int alpha, int beta, uint8_t depth);

#endif /* BRAIN_H_ */
