/*
 * pregame.c
 * 
 * Implementations of the functions described in pregame.h
 * 
 * @author Daniel Rogers
 *
 */

#include "pregame.h"

/*
 * Generates the table of valid moves for each piece/position in addition to
 * the bitboards representing attacking squares for each piece/position. The
 * resulting table will be exported as a binary file to be used by
 * loadMoveTables()
 *
 * @modifies legal_moves, attacked_squares in board.h
 */
void generateMoveTable()
{

}

/*
 * Loads in a binary file created by generateMoveTable to initializer the
 * pre-calculated move tables.
 *
 * @modifies legal_moves, attacked_squares in board.h
 *
 * @returns true if the file loaded successfully, false if some error occurred
 *          which prevented a successful initialization of legal_moves and
 *          attacked_squares.
 */
bool loadMoveTables()
{
    return (false);
}

/*
 * Populates the hashkey table for the board with randomly generated N-bit
 * values, where N is the bitwidth of HASHKEY.
 *
 * @modifies key_table in board.h
 */
void generateHashkeys()
{

}

/*
 * Calculates the moves available to a pawn piece from a location
 *
 * @owner Daniel Rogers
 *
 * @param location The location of the pawn
 * @param moves An array to fill with the available moves from that location
 *              They will be in order from left to right
 *              (from white's perspective)
 * @param white true if calculating moves for the white pieces
 */
void calcPawnMoves(uint8_t location, uint8_t moves[3], bool white)
{

}

/*
 * Calculates the moves available to a knight piece from a location
 *
 * @param location The location of the knight
 * @param moves An array to fill with the available moves from that location
 *              They will travel clockwise from the top-right
 *              (from white's perspective)
 * @param white true if calculating moves for the white pieces
 */
void calcKnightMoves(uint8_t location, uint8_t moves[8], bool white)
{

}

/*
 * Calculates the moves available to a bishop piece from a location
 *
 * @param location The location of the bishop
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 * @param white true if calculating moves for the white pieces
 */
void calcBishopMoves(uint8_t location, uint8_t moves[4][7], bool white)
{

}

/*
 * Calculates the moves available to a rook piece from a location
 *
 * @param location The location of the rook
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 * @param white true if calculating moves for the white pieces
 */
void calcRookMoves(uint8_t location, uint8_t moves[4][7], bool white)
{

}

/*
 * Calculates the moves available to a queen piece from a location
 *
 * @param location The location of the queen
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 * @param white true if calculating moves for the white pieces
 */
void calcQueenMoves(uint8_t location, uint8_t moves[8][7], bool white)
{

}

/*
 * Calculates the moves available to a king from a location
 *
 * @param location The location of the king
 * @param moves An array to fill with the available moves from that location.
 *              Each array will be filled with the moves available to the
 *              piece, traveling clockwise from the top
 *              (from white's perspective)
 * @param white true if calculating moves for the white pieces
 */
void calcKingMoves(uint8_t location, uint8_t moves[8], bool white)
{

}

