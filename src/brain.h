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
 *                       states. These may be realloc'd durring execution of
 *                       the search.
 *                       !!! It is assumed to contain at least as many
 *                       pointers as the maximum depth of the search. Supplying
 *                       less will result in out of bounds memory access. !!!
 * @param depth The depth to traverse to
 *
 * @return The best score resulting from the negamax search
 */
int negamax(chessboard * state, bool white, chessboard ** expansionStore,
        uint8_t depth);

#endif /* BRAIN_H_ */
