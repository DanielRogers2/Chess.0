/*
 * board.h
 *
 * Contains the function definitions & data structures for manipulating the
 * board state.
 *
 * @author Daniel Rogers
 *
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "common_defs.h"
#include "globals.h"

//Piece code definitions
#define W_P 0
#define W_R 1
#define W_N 2
#define W_B 3
#define W_Q 4
#define W_K 5

#define B_P 6
#define B_R 7
#define B_N 8
#define B_B 9
#define B_Q 10
#define B_K 11

/*
 * Defines an overall board state for the program
 *
 * Positions are coded from the white side of the board starting at a1 (0) up
 * to h8 (63).
 *
 * A value of 64 indicates a captured piece
 *
 */
typedef struct
{
    //Locations of all white pieces
    uint8_t w_pieces[16];
    //Bitboard of all white pieces
    bitboard all_w_pieces;
    //Bitboards of individual piece locations for white
    bitboard w_locations[16];

    //Locations of all black pieces
    uint8_t b_pieces[16];
    //Bitboard of all black pieces
    bitboard all_b_pieces;
    //Bitboards of individual piece locations for black
    bitboard b_locations[16];
} chessboard;

/*
 * Initializes the board to its base state, where no moves have been made yet
 *
 * @owner Daniel Rogers
 *
 * @uses white_initial, black_initial, w_codes, b_codes
 *
 * @param board The chessboard to initialize.
 */
void initBoard(chessboard * board);

/*
 * Expands the set of all possible board states from an initial state
 *
 * @owner Daniel Rogers
 *
 * @uses w_codes, b_codes, location_boards, legal_moves
 *
 * @param board A pointer to the board to expand
 * @param storage A pointer to an array in which to store the expanded states
 *         !!This pointer will be realloc'd to fit the set of expanded states!!
 * @param white true if expanding the set of white moves
 * @return The number of states expanded
 */
uint8_t expandStates(chessboard * const board, chessboard * storage, bool white);

/*
 * Generates a new board state based on a piece move
 *
 * @owner Daniel Rogers
 *
 * @param piece The index of the piece to move
 * @param location The location to move to
 * @param white true If the piece being moved is white
 * @param current The chessboard state being referenced
 * @param new The new chessboard state to write to
 */
void makeMove(uint8_t piece, uint8_t location, bool white,
        chessboard * const current, chessboard * new);

/*
 * Evaluates the value of a particular board, where higher values are more
 *  favorable towards white
 *
 * @owner Daniel Rogers
 *
 * @uses all of the *_*_positions globals
 *
 * @param board The board to evaluate
 * @return The value of the board in a form usable in a negamax function
 */
int evaluateStateWhite(chessboard * const board);

/*
 * Evaluates the value of a particular board, where higher values are more
 *  favorable towards black
 *
 * @owner Daniel Rogers
 *
 * @uses all of the *_*_positions globals
 *
 * @param board The board to evaluate
 * @return The value of the board in a form usable in a negamax function
 */
int evaluateStateBlack(chessboard * const board);

#endif /* BOARD_H_ */
