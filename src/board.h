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
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ON ((uint64_t) 1)

#define INVALID_SQUARE ((uint8_t) 64)
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
 * Lookup table indicating the location bitboards for each square
 *
 * location_boards[64] is set to UINT64_MAX so that a logical and of any non-0
 * bitboard will result in a non-0 value
 *
 * @users this, pregame
 */
static const bitboard location_boards[65] =
{ ON, ON << 1, ON << 2, ON << 3, ON << 4, ON << 5, ON << 6, ON << 7,
ON << 8, ON << 9, ON << 10, ON << 11, ON << 12, ON << 13, ON << 14, ON << 15,
ON << 16, ON << 17, ON << 18, ON << 19, ON << 20, ON << 21, ON << 22, ON << 23,
ON << 24, ON << 25, ON << 26, ON << 27, ON << 28, ON << 29, ON << 30, ON << 31,
ON << 32, ON << 33, ON << 34, ON << 35, ON << 36, ON << 37, ON << 38, ON << 39,
ON << 40, ON << 41, ON << 42, ON << 43, ON << 44, ON << 45, ON << 46, ON << 47,
ON << 48, ON << 49, ON << 50, ON << 51, ON << 52, ON << 53, ON << 54, ON << 55,
ON << 56, ON << 57, ON << 58, ON << 59, ON << 60, ON << 61, ON << 62, ON << 63,
UINT64_MAX };

/*
 * This defines a mapping between piece location bitboard indexes and piece codes
 * Pieces are coded as follows:
 *  piece . w . b
 *  Pawn:   0   6
 *  Rook:   1   7
 *  Knight: 2   8
 *  Bishop: 3   9
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
 * Initial positions of white/black pieces
 * Pawns:   indexes 0-7 of white/black position arrays
 *   8-15, 48-55
 * Rook:    indexes 08-09, squares 0,7 & 56,63
 * Knights: indexes 10-11, squares 1,6 & 57,62
 * Bishops: indexes 12-13, squares 2,5 & 58,61
 * Queen:   index 14, squares 3 & 59
 * King:    index 15, squares 4 & 60
 *
 * @users this
 */
static const uint8_t white_initial[16] =
{ 8, 9, 10, 11, 12, 13, 14, 15, 0, 7, 1, 6, 2, 5, 3, 4 };
static const uint8_t black_initial[16] =
{ 48, 49, 50, 51, 52, 53, 54, 55, 56, 63, 57, 62, 58, 61, 59, 60 };

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

#endif /* BOARD_H_ */
