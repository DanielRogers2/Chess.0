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
//white pawn
#define W_P 0
//white rook
#define W_R 1
//white knight
#define W_N 2
//white bishop
#define W_B 3
//white queen
#define W_Q 4
//white king
#define W_K 5

//black pawn
#define B_P 6
//black rook
#define B_R 7
//black knight
#define B_N 8
//black bishop
#define B_B 9
//black queen
#define B_Q 10
//black king
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
    //White Piece IDS
    uint8_t w_codes[16];

    //Locations of all black pieces
    uint8_t b_pieces[16];
    //Bitboard of all black pieces
    bitboard all_b_pieces;
    //Bitboards of individual piece locations for black
    bitboard b_locations[16];
    //White Piece IDS
    uint8_t b_codes[16];

    //Tracking data
    //Last piece moved (as index of array)
    uint8_t last_piece;
    //Last location moved to
    uint8_t last_move;

} chessboard;

typedef struct
{
    uint8_t count;
    chessboard * data;
} boardset;

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

//TODO Add function to convert opponent move string into a new board state
//TODO Add function to convert own move into a move string to send to server

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
uint8_t expandStates(chessboard * const board, boardset * storage, bool white);

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
 * Handles making special moves such as castling/en passant/promotion
 *
 * !!This function should ONLY BE CALLED if a special move has been made,
 *   it is only responsible for handling board state updating, NOT validating
 *   if a move is special or not!!
 *
 * @param piece The index of the piece to move
 * @param location The location to move to
 * @param white true If the piece being moved is white
 * @param current The chessboard state being referenced
 * @param new The new chessboard state to write to
 * @param promote_to If promoting, this is the piece code of the desired
 *                   promotion
 */
void moveSpecial(uint8_t piece, uint8_t location, bool white,
        chessboard * const current, chessboard * new, uint8_t promote_to);

/*
 * Evaluates the value of a particular board
 *
 * @owner Daniel Rogers
 *
 * Based on:
 *   http://chessprogramming.wikispaces.com/Simplified+evaluation+function
 *
 * @uses all of the *_*_positions globals
 *
 * @param board The board to evaluate
 * @param white True if the call is being made from white's perspective
 * @return The value of the board in a form usable in a negamax function
 */
int evaluateState(chessboard * const board, bool white);

#endif /* BOARD_H_ */
