/*
 * common_defs.h
 * 
 * 
 * 
 * @author Js
 * 
 */

#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

#define ON ((uint64_t) 1)

#define INVALID_SQUARE ((uint8_t) 64)
#define CAPTURED INVALID_SQUARE

/*
 * This is a bitmap of the board, used to represent various boolean predicates
 * examples:
 *      white/black piece here?
 *      squares attacked by piece from location
 *
 */
typedef uint64_t bitboard;

#endif /* COMMON_DEFS_H_ */
