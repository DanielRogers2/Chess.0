/*
 * globals.c
 * 
 * 
 * 
 * @author Daniel Rogers
 * 
 */

#include <inttypes.h>
#include "common_defs.h"

/*
 * At runtime, this will be pointed at a pre-generated table of valid moves for
 * a piece, given the piece and its location. This will include moves that are
 * only valid for a pawn if it is capturing.
 *
 * legal_moves[piece_code][position][direction (from white's perspective)][moves]
 *
 * A value of 64 in [moves] indicates an invalid move, and the end of a ray.
 *
 * @users board
 * @modifiers pregame
 *
 * @initializer pregame->generateMoveTable. This creates a binary file that
 *              will be used to initialize the table for later runs of the
 *              program.
 * @initializer pregame->loadMoveTables. This loads a pre-generated binary
 *              file into this variable
 */
uint8_t legal_moves[12][64][8][7];

/*
 * At runtime this will point to a pre-generated table of attacked squares
 * given a piece and location
 *
 * attacked_squares[piece_code][position]
 *
 * @users board
 * @modifiers pregame
 *
 * @initializer pregame->generateMoveTable. This creates a binary file that
 *              will be used to initialize the table for later runs of the
 *              program.
 * @initializer pregame->loadMoveTables. This loads a pre-generated binary
 *              file into this variable
 */
bitboard attacked_squares[12][64];

/*
 * Lookup table indicating the location bitboards for each square
 *
 * location_boards[64] is set to UINT64_MAX so that a logical and of any non-0
 * bitboard will result in a non-0 value
 *
 * @users board
 * @users pregame
 */
const bitboard location_boards[65] =
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
 * @users board
 * @users pregame
 */
const uint8_t w_codes[16] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5 };
const uint8_t b_codes[16] =
{ 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11 };

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
 * @users board
 */
const uint8_t white_initial[16] =
{ 8, 9, 10, 11, 12, 13, 14, 15, 0, 7, 1, 6, 2, 5, 3, 4 };
const uint8_t black_initial[16] =
{ 48, 49, 50, 51, 52, 53, 54, 55, 56, 63, 57, 62, 58, 61, 59, 60 };

/*
 * Stores the randomly generated N-bit keys for hash table key generation.
 * The resulting hash table will contain 2^N entries. This is indexed by piece
 * type and location.
 *
 * These are loaded at program start, and not modified during runtime unless
 * game pre-generation work is being performed.
 *
 * @users board
 * @modifiers pregame
 *
 * @initializer pregame->generateHashkeys. Generates a new set of hashkeys
 *              prior to game start. Hashkeys are not currently saved, and
 *              must be regenerated during each startup.
 */
HASHKEY key_table[12][64];

