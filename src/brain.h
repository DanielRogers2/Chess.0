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
 * @owner Daniel Rogers
 *
 * @param state The start state for the layer
 * @param expansionStore An array of chessboard * to use for storing expanded
 *                       states. These may be realloc'd durring execution of
 *                       the search.
 *                       !!! It is assumed to contain at least as many
 *                       pointers as the maximum depth of the search. Supplying
 *                       less will result in out of bounds memory access. !!!
 * @param depth The depth to traverse to
 * @param choice A chessboard * in which to store the resulting best state
 *
 * @return The best score resulting from the negamax search
 */
int negamax(chessboard state, chessboard ** expansionStore, uint8_t depth,
        chessboard * choice);

#endif /* BRAIN_H_ */
