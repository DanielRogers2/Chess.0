/*
 * board.h
 *
 * Contains the function definitions & data structures for manipulating the
 * board state
 *
 * @author Daniel Rogers
 *
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

/*
 * At runtime, this will be pointed at a pre-generated table of valid moves for
 * a piece, given the piece and its location. This will include moves that are
 * only valid for a pawn if it is capturing.
 *
 * legal_moves[piece_code][position][direction (from white's perspective)][moves]
 *
 * A value of 64 in [moves] indicates an invalid move, and the end of a ray.
 *
 * used here
 * used in pregame.h
 */
static uint8_t legal_moves[12][64][8][7];

/*
 * This is a bitmap of the board, used to represent various boolean predicates
 * examples:
 *      white/black piece here?
 *      squares attacked by piece from location
 *
 */
typedef uint64_t bitboard;

/*
 * At runtime this will point to a pre-generated table of attacked squares
 * given a piece and location
 *
 * attacked_squares[piece_code][position]
 *
 * used here
 * used in pregame.h
 */
static bitboard attacked_squares[12][64];

/*
 * This defines a mapping between piece location bitboard indexes and piece codes
 * Pieces are coded as follows:
 *  piece . w . b
 *  Pawn:   0   6
 *  Bishop: 1   7
 *  Knight: 2   8
 *  Rook:   3   9
 *  Queen:  4   10
 *  King:   5   11
 *
 * used here
 * used in pregame.h
 */
static const uint8_t w_codes[16] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5 };
static const uint8_t b_codes[16] =
{ 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11 };

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
 * Stores the randomly generated N-bit keys for hash table key generation.
 * The resulting hash table will contain 2^N entries. This is indexed by piece
 * type and location.
 *
 * These are loaded at program start, and not modified during runtime unless
 * game pre-generation work is being performed.
 *
 * used here
 * used in pregame.h
 */
static HASHKEY key_table[12][64];

#endif /* BOARD_H_ */
