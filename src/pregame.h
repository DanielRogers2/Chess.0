/*
 * pregame.h
 *
 * Contains the functions associated with pre-game generation work & loading
 * of pre-generated structures.
 *
 * @author Daniel Rogers
 * 
 */

#ifndef PREGAME_H_
#define PREGAME_H_

#include <stdint.h>
#include <stdbool.h>
#include "board.h"

/*
 * Generates the table of valid moves for each piece/position in addition to
 * the bitboards representing attacking squares for each piece/position
 *
 * Modifies: legal_moves, attacked_squares in board.h
 */
void generateMoveTable();

/*
 * Populates the hashkey table for the board
 *
 * Modifies: key_table in board.h
 */
void generateHashkeys();

/*
 * Calculates the moves available to a pawn piece from a location
 *
 * @param location The location of the pawn
 * @param moves An array to fill with the available moves from that location
 *              They will be in order from left to right
 *              (from white's perspective)
 * @param white True if calculating moves for the white pieces
 */
void calcPawnMoves(uint8_t location, uint8_t moves[3], bool white);

/*
 * Calculates the moves available to a knight piece from a location
 *
 * @param location The location of the knight
 * @param moves An array to fill with the available moves from that location
 *              They will travel clockwise from the top-right
 *              (from white's perspective)
 * @param white True if calculating moves for the white pieces
 */
void calcKnightMoves(uint8_t location, uint8_t moves[8], bool white);

/*
 * Calculates the moves available to a bishop piece from a location
 *
 * @param location The location of the bishop
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 * @param white True if calculating moves for the white pieces
 */
void calcBishopMoves(uint8_t location, uint8_t moves[4][7], bool white);

/*
 * Calculates the moves available to a rook piece from a location
 *
 * @param location The location of the rook
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 * @param white True if calculating moves for the white pieces
 */
void calcRookMoves(uint8_t location, uint8_t moves[4][7], bool white);

/*
 * Calculates the moves available to a queen piece from a location
 *
 * @param location The location of the queen
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 * @param white True if calculating moves for the white pieces
 */
void calcQueenMoves(uint8_t location, uint8_t moves[8][7], bool white);

/*
 * Calculates the moves available to a king from a location
 *
 * @param location The location of the king
 * @param moves An array to fill with the available moves from that location.
 *              Each array will be filled with the moves available to the
 *              piece, traveling clockwise from the top
 *              (from white's perspective)
 * @param white True if calculating moves for the white pieces
 */
void calcKingMoves(uint8_t location, uint8_t moves[8], bool white);

#endif /* PREGAME_H_ */
