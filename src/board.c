/*
 * board.c
 * 
 * Implementations of the functions defined in board.h
 * 
 * @author Daniel Rogers
 * 
 */

#include "board.h"

/*
 * Initializes the board to its base state, where no moves have been made yet
 *
 * @uses white_initial, black_initial, location_bitboards
 *
 * @owner Daniel Rogers
 *
 * @param board The chessboard to initialize.
 */
void initBoard(chessboard * board)
{
    //Set up the occupation bitboards for all white/black pieces
    //White pieces are in positions 0-15 -> low 16 bits on
    board->all_w_pieces = 0xFFFF;
    //Black pieces are in positions 48-63 -> low 48 off
    board->all_b_pieces = ((bitboard) 0xFFFF) << 48;

    //Set location arrays
    memcpy(board->w_pieces, white_initial, 16 * sizeof(uint8_t));
    memcpy(board->b_pieces, black_initial, 16 * sizeof(uint8_t));

    //Set location bitboards
    for (uint8_t i = 0; i < 16; ++i)
    {
        board->w_locations[i] = location_boards[board->w_pieces[i]];
        board->b_locations[i] = location_boards[board->b_pieces[i]];
    }
}

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
uint8_t expandStates(chessboard * const board, chessboard * storage, bool white)
{
    //Select the appropriate sets of data
    //Piece locations
    uint8_t * pieces = (white) ? board->w_pieces : board->b_pieces;
    //Side location occupancy boards
    bitboard self = (white) ? board->all_w_pieces : board->all_b_pieces;
    //Lookup table piece codes
    const uint8_t * codes = (white) ? w_codes : b_codes;

    //Number of states generated
    uint8_t states = 0;

    //Loop variables
    uint8_t i, j, k;

    //The set of moves
    uint8_t (*moves)[7];

    //Allocate enough storage
    realloc(storage, 35 * sizeof(chessboard));

    //For each piece, get the set of moves it can make from its location
    for (i = 0; i < 16; ++i)
    {
        //Check if piece is captured
        if (pieces[i] == 64)
        {
            continue;
        }

        moves = legal_moves[codes[i]][pieces[i]];
        //Go through each move ray
        for (j = 0; j < 8; ++j)
        {
            //Go through each move in ray
            for (k = 0; k < 7; ++k)
            {
                //Check for end of ray, or own piece @ location
                //Location check works as follows:
                //  Get the bitboard representing the destination
                //  Then AND with pieces for own side -> all 0 if no pieces at
                //      destination, meaning valid move. !0 if own piece at
                //      location, meaning invalid move
                //location_boards[64] == 0xffffffffffffffff, so any AND will
                //  make a non-0 value
                if (location_boards[moves[j][k]] & self)
                {
                    //Stop looking through ray
                    break;
                }
                else
                {
                    //Make the move with the piece
                    makeMove(i, moves[j][k], white, board, &storage[states++]);
                    if (((states % 35) == 0))
                    {
                        //Allocate more storage
                        realloc(storage, (states + 35) * sizeof(chessboard));
                    }
                }
            }
        }
    }

    return (states);
}

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
        chessboard * const current, chessboard * new)
{
    //Generate the new location bitboard for the new location
    bitboard new_loc = location_boards[location];

    //copy data
    memcpy(new, current, sizeof(chessboard));

    //Update the location
    if (white)
    {
        //Update the occupancy bitboard
        //XOR out the old location, XOR in the new location
        new->all_w_pieces ^= (new->w_locations[piece] ^ new_loc);
        //Update position bitboard
        new->w_locations[piece] = new_loc;
        //update piece location
        new->w_pieces[piece] = location;
    }
    else
    {
        //for black pieces
        //Update the occupancy bitboard
        //XOR out the old location, XOR in the new location
        new->all_b_pieces ^= (new->b_locations[piece] ^ new_loc);
        //Update position bitboard
        new->b_locations[piece] = new_loc;
        //update piece location
        new->b_pieces[piece] = location;
    }
}
