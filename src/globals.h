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
extern const char piece_chars[12];
extern const uint16_t piece_vals[12];
extern const uint8_t white_initial[16];
extern const uint8_t black_initial[16];
extern const int8_t w_P_positions[64];
extern const int8_t b_P_positions[64];
extern const int8_t w_N_positions[64];
extern const int8_t b_N_positions[64];
extern const int8_t w_B_positions[64];
extern const int8_t b_B_positions[64];
extern const int8_t w_R_positions[64];
extern const int8_t b_R_positions[64];
extern const int8_t w_Q_positions[64];
extern const int8_t b_Q_positions[64];
extern const int8_t w_K_m_positions[64];
extern const int8_t b_K_m_positions[64];
extern const int8_t w_K_e_positions[64];
extern const int8_t b_K_e_positions[64];
extern const int8_t * board_position_vals[12];
//extern HASHKEY key_table[12][64];

#endif /* GLOBALS_H_ */
