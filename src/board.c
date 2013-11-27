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
 * Expands the set of all possible board states from an initial state
 *
 * @owner Daniel Rogers
 *
 * @param board A pointer to the board to expand
 * @param storage A pointer to an array in which to store the expanded states
 *         !!This pointer will be realloc'd to fit the set of expanded states!!
 * @return The number of states expanded
 */
uint8_t expandStates(chessboard * const board, chessboard * storage)
{
    return (0);
}

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
        chessboard * const current, chessboard * new)
{
    //Generate the new location bitboard for the new location
    bitboard new_loc = 1 << location;

    //copy data
    memcpy(new, current, sizeof(chessboard));

    //Update the location
    if (white)
    {
        //Update the occupancy bitboard
        //XOR out the old location, XOR in the new location
        new->all_w_pieces ^= (new->w_locations[piece] ^ new_loc);
        //Update position bitboard
        new->w_locations[piece] = new_loc;
        //update piece location
        new->w_pieces[piece] = location;
    }
    else
    {
        //for black pieces
        //Update the occupancy bitboard
        //XOR out the old location, XOR in the new location
        new->all_b_pieces ^= (new->b_locations[piece] ^ new_loc);
        //Update position bitboard
        new->b_locations[piece] = new_loc;
        //update piece location
        new->b_pieces[piece] = location;
    }
}
