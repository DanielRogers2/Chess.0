/*
 * globals.h
 * 
 * 
 * 
 * @author Daniel Rogers
 * 
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "common_defs.h"

//declare external
extern uint8_t legal_moves[12][64][8][7];
extern bitboard attacked_squares[12][64];
extern const bitboard location_boards[65];
extern const uint8_t w_codes[16];
extern const uint8_t b_codes[16];
extern const uint8_t white_initial[16];
extern const uint8_t black_initial[16];
extern HASHKEY key_table[12][64];

#endif /* GLOBALS_H_ */
