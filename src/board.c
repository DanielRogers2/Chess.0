/*
 * board.c
 * 
 * Implementations of the functions defined in board.h
 * 
 * @author Daniel Rogers
 * 
 */

#include "board.h"

/*
 * Initializes the board to its base state, where no moves have been made yet
 *
 * @uses white_initial, black_initial, w_codes, b_codes
 *
 * @owner Daniel Rogers
 *
 * @param board The chessboard to initialize.
 */
void initBoard(chessboard * board)
{
    //Set up the occupation bitboards for all white/black pieces
    //White pieces are in positions 0-15 -> low 16 bits on
    board->all_w_pieces = 0xFFFF;
    //Black pieces are in positions 48-63 -> low 48 off
    board->all_b_pieces = ((bitboard) 0xFFFF) << 48;

    //Set location arrays
    memcpy(board->w_pieces, white_initial, 16 * sizeof(uint8_t));
    memcpy(board->b_pieces, black_initial, 16 * sizeof(uint8_t));

    //Set location bitboards
    for (uint8_t i = 0; i < 16; ++i)
    {
        board->w_locations[i] = ON << board->w_pieces[i];
        board->b_locations[i] = ON << board->b_pieces[i];
    }
}

/*
 * Generates a new board state based on a piece move
 *
 * @owner Daniel Rogers
 *
 * @param piece The index of the piece to move
 * @param white true If the piece being moved is white
 * @param current The chessboard state being referenced
 * @param new The new chessboard state to write to
 */
void makeMove(uint8_t piece, bool white, chessboard * const current,
        chessboard * new)
{
    //TODO Implement this
}
