/*
 * board.h
 *
 * Contains the function definitions & data structures for manipulating the
 * board state.
 *
 * @modifiers pregame
 *
 * @author Daniel Rogers
 *
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#define INVALID_SQUARE ((uint8_t) 64)
//Piece code definitions
#define W_P 0
#define W_B 1
#define W_N 2
#define W_R 3
#define W_Q 4
#define W_K 5

#define B_P 6
#define B_B 7
#define B_N 8
#define B_R 9
#define B_Q 10
#define B_K 11

/*
 * At runtime, this will be pointed at a pre-generated table of valid moves for
 * a piece, given the piece and its location. This will include moves that are
 * only valid for a pawn if it is capturing.
 *
 * legal_moves[piece_code][position][direction (from white's perspective)][moves]
 *
 * A value of 64 in [moves] indicates an invalid move, and the end of a ray.
 *
 * @users this
 * @modifiers pregame
 *
 * @initializer pregame->generateMoveTable. This creates a binary file that
 *              will be used to initialize the table for later runs of the
 *              program.
 * @initializer pregame->loadMoveTables. This loads a pre-generated binary
 *              file into this variable
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
 * @users this
 * @modifiers pregame
 *
 * @initializer pregame->generateMoveTable. This creates a binary file that
 *              will be used to initialize the table for later runs of the
 *              program.
 * @initializer pregame->loadMoveTables. This loads a pre-generated binary
 *              file into this variable
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
 * @users this
 * @users pregame
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
 * @users this
 * @modifiers pregame
 *
 * @initializer pregame->generateHashkeys. Generates a new set of hashkeys
 *              prior to game start. Hashkeys are not currently saved, and
 *              must be regenerated during each startup.
 */
static HASHKEY key_table[12][64];

/*
 * Initializes the board to its base state, where no moves have been made yet
 *
 * @owner Daniel Rogers
 *
 * @param board The chessboard to initialize.
 */
void initBoard(chessboard * board);

#endif /* BOARD_H_ */
