/*
 * pregame.h
 *
 * Contains the functions associated with pre-game generation work & loading
 * of pre-generated structures.
 *
 * @author Js
 * 
 */

#ifndef PREGAME_H_
#define PREGAME_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "common_defs.h"
#include "globals.h"
#include "board.h"
/*
 * Generates the table of valid moves for each piece/position in addition to
 * the bitboards representing attacking squares for each piece/position. The
 * resulting table will be exported as a binary file to be used by
 * loadMoveTables()
 *
 * @owner Js
 *
 * @modifies legal_moves, attacked_squares
 */
void generateMoveTables();

/*
 * Loads in a binary file created by generateMoveTable to initializer the
 * pre-calculated move tables.
 *
 * @owner Js
 *
 * @modifies legal_moves, attacked_squares
 *
 * @returns true if the file loaded successfully, false if some error occurred
 *          which prevented a successful initialization of legal_moves and
 *          attacked_squares.
 */
bool loadMoveTables();

/*
 * Populates the hashkey table for the board with randomly generated N-bit
 * values, where N is the bitwidth of HASHKEY.
 *
 * @modifies key_table
 void generateHashkeys();
 */

/*
 * Calculates the moves available to a pawn piece from a location
 *
 * @owner Js
 *
 * @uses location_boards
 *
 * @param location The location of the pawn
 * @param moves An array to fill with the available moves from that location
 *              They will be in order from left to right
 *              (from white's perspective).
 *              Assumes that it is preset to INVALID
 * @param atk_bboard The attack bitboard to configure for the location
 *                      It is assumed to be set to 0.
 *                      Does not account for en passant captures
 * @param white true if calculating moves for the white pieces
 */
void calcPawnMoves(uint8_t location, uint8_t moves[8][7], bitboard atkbboard,
bool white);

/*
 * Calculates the moves available to a knight piece from a location
 *
 * @owner Js
 *
 * @uses location_boards
 *
 * @param location The location of the knight
 * @param moves An array to fill with the available moves from that location
 *              They will travel clockwise from the top-right
 *              (from white's perspective)
 *              Assumes that it is preset to INVALID
 * @param atk_bboard The attack bitboard to configure for the location
 *                      It is assumed to be set to 0.
 */
void calcKnightMoves(uint8_t location, uint8_t moves[8][7], bitboard atkbboard);

/*
 * Calculates the moves available to a bishop piece from a location
 *
 * @owner Js
 *
 * @uses location_boards
 *
 * @param location The location of the bishop
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 *              Assumes that it is preset to INVALID
 * @param atk_bboard The attack bitboard to configure for the location
 *                      It is assumed to be set to 0.
 */
void calcBishopMoves(uint8_t location, uint8_t moves[4][7], bitboard atkbboard);

/*
 * Calculates the moves available to a rook piece from a location
 *
 * @owner Js
 *
 * @uses location_boards
 *
 * @param location The location of the rook
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 *              Assumes that it is preset to INVALID
 * @param atk_bboard The attack bitboard to configure for the location
 *                      It is assumed to be set to 0.
 */
void calcRookMoves(uint8_t location, uint8_t moves[4][7], bitboard atkbboard);

/*
 * Calculates the moves available to a queen piece from a location
 *
 * @owner Js
 *
 * @uses location_boards
 *
 * @param location The location of the queen
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 *              Assumes that it is preset to INVALID
 * @param atk_bboard The attack bitboard to configure for the location
 *                      It is assumed to be set to 0.
 */
void calcQueenMoves(uint8_t location, uint8_t moves[8][7], bitboard atkbboard);

/*
 * Calculates the moves available to a king from a location
 *
 * @owner Js
 *
 * @uses location_boards
 *
 * @param location The location of the king
 * @param moves An array to fill with the available moves from that location.
 *              Each array will be filled with the moves available to the
 *              piece, traveling clockwise from the top
 *              (from white's perspective)
 *              Assumes that it is preset to INVALID
 * @param atk_bboard The attack bitboard to configure for the location
 *                      It is assumed to be set to 0.
 */
void calcKingMoves(uint8_t location, uint8_t moves[8][7], bitboard atkbboard);

#endif /* PREGAME_H_ */
