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
 * @uses white_initial, black_initial, location_bitboards, w_codes, b_codes
 *
 * @owner Daniel Rogers
 *
 * @param board The chessboard to initialize.
 */
void initBoard(chessboard * board)
{
    //Set up the occupation bitboards for all white/black pieces
    //White pieces are in positions 0-15 -> low 16 bits on
    board->all_w_pieces = 0;
    //Black pieces are in positions 48-63 -> low 48 off
    board->all_b_pieces = 0;

    //Set location arrays
    memcpy(board->w_pieces, white_initial, 16 * sizeof(uint8_t));
    memcpy(board->b_pieces, black_initial, 16 * sizeof(uint8_t));

    //Set initial piece codes
    memcpy(board->w_codes, w_codes, 16 * sizeof(uint8_t));
    memcpy(board->b_codes, b_codes, 16 * sizeof(uint8_t));

    //Set location bitboards
    for (uint8_t i = 0; i < 16; ++i)
    {
        board->w_locations[i] = location_boards[board->w_pieces[i]];
        board->b_locations[i] = location_boards[board->b_pieces[i]];
        //Update population bitboard
        board->all_w_pieces |= board->w_locations[i];
        board->all_b_pieces |= board->b_locations[i];
    }

#ifdef DEBUG_INIT
    for (uint8_t i = 0; i < 16; ++i)
    {
        printf("w%d: %d\n", i, board->w_pieces[i]);
        printf("wloc%d: %llx\n", i, board->w_locations[i]);
        printf("b%d: %d\n", i, board->b_pieces[i]);
        printf("bloc%d: %llx\n", i, board->b_locations[i]);
    }
    printf("woc: %llx\n", board->all_w_pieces);
    printf("boc: %llx\n", board->all_b_pieces);
#endif
}

/*
 * Expands the set of all possible board states from an initial state
 *
 * @owner Daniel Rogers
 *
 * @uses location_boards, legal_moves
 *
 * @param board A pointer to the board to expand
 * @param storage A pointer to an array in which to store the expanded states
 *         !!This pointer will be realloc'd to fit the set of expanded states!!
 * @param white true if expanding the set of white moves
 * @return The number of states expanded
 */
uint8_t expandStates(chessboard * const board, boardset * storage, bool white)
{
    //Select the appropriate sets of data
    //Piece locations
    uint8_t * pieces = (white) ? board->w_pieces : board->b_pieces;
    //Side location occupancy boards
    bitboard self = (white) ? board->all_w_pieces : board->all_b_pieces;
    bitboard op = (white) ? board->all_b_pieces : board->all_w_pieces;
    //Lookup table piece codes
    const uint8_t * codes = (white) ? board->w_codes : board->b_codes;

    //Number of states generated
    uint8_t states = 0;

    //Loop variables
    uint8_t i, j, k;
    //The set of moves
    uint8_t (*moves)[7];

    if (storage->count < 35)
    {
        //Allocate enough storage
        storage->data = realloc(storage->data, 35 * sizeof(chessboard));
        storage->count = 35;
    }

    //For each piece, get the set of moves it can make from its location
    for (i = 0; i < 16; ++i)
    {
        //Check if piece is captured
        if (pieces[i] == CAPTURED)
        {
            continue;
        }

#ifdef DEBUG_MOVE
        printf("piece: %d, @%d\n", codes[i], pieces[i]);
        printf("move0: %d\n", legal_moves[codes[i]][pieces[i]][0][0]);
#endif

        moves = legal_moves[codes[i]][pieces[i]];
        //Go through each move ray
        for (j = 0; j < 8; ++j)
        {
            //Go through each move in ray
            for (k = 0; k < 7; ++k)
            {
#ifdef DEBUG_MOVE
                printf("making move: %d\n", moves[j][k]);
#endif
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
#ifdef DEBUG_MOVE
                    puts("breaking");
#endif
                    break;
                }
                else if ((codes[i] == W_P || codes[i] == B_P)
                        && (((j != 0) && (!(location_boards[moves[j][k]] & op)))
                                || ((j == 0)
                                        && (location_boards[moves[j][k]] & op))))
                {
                    //  If it's a pawn, diagonals are only allowed on capture
                    //  Can't capture by moving forward
#ifdef DEBUG_MOVE
                    puts("breaking");
#endif
                    break;
                }
                else
                {
                    //Make the move with the piece
                    makeMove(i, moves[j][k], white, board,
                            &storage->data[states++]);

                    if (storage->count <= states)
                    {
                        //Allocate more storage
                        storage->count += 10;
                        storage->data = realloc(storage->data,
                                storage->count * sizeof(chessboard));
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

    //Update last move
    new->last_move = location;
    new->last_piece = piece;

    //Set up data pointers
    //All pieces
    bitboard * self_all = (white) ? &new->all_w_pieces : &new->all_b_pieces;
    bitboard * op_all = (white) ? &new->all_b_pieces : &new->all_w_pieces;
    //location bitboards
    bitboard * self_locs = (white) ? new->w_locations : new->b_locations;
    bitboard * op_locs = (white) ? new->b_locations : new->w_locations;
    //piece value
    uint8_t * self_pcs = (white) ? new->w_pieces : new->b_pieces;
    uint8_t * op_pcs = (white) ? new->b_pieces : new->w_pieces;

    //Update the occupancy bitboard
    //XOR out the old location, XOR in the new location
    *self_all ^= (self_locs[piece] ^ new_loc);
    //Update position bitboard
    self_locs[piece] = new_loc;
    //update piece location
    self_pcs[piece] = location;

    //Handle captures, capturing if opponent piece @ location
    //  (maybe should be in own function?)
    if ((*op_all) & new_loc)
    {
        for (uint8_t i = 0; i < 16; ++i)
        {
            //If opponent piece at same location as new location
            if (op_locs[i] == new_loc)
            {
                //XOR out location in occupancy board
                *op_all ^= op_locs[i];
                //Set to invalid position
                op_locs[i] = 0;
                //Flag as captured
                op_pcs[i] = CAPTURED;
                //done
                break;
            }
        }
    }
}

/*
 * Handles making special moves such as castling/en passant/promotion
 *
 * !!This function should ONLY BE CALLED if a special move has been made,
 *   it is only responsible for handling board state updating, NOT validating
 *   if a move is special or not!!
 *
 * @uses *_initial to determine index of rook for castling maneuver
 *
 * @owner Daniel Rogers
 *
 * @param piece The index of the piece to move
 * @param location The location to move to
 * @param white true If the piece being moved is white
 * @param current The chessboard state being referenced
 * @param new The new chessboard state to write to
 * @param promote_to If promoting, this is the piece code of the desired
 *                   promotion
 */
void moveSpecial(uint8_t piece, uint8_t location, bool white,
        chessboard * const current, chessboard * new, uint8_t promote_to)
{
    //do initial work with makeMove
    //Capturing during a pawn promotion move will be handled by this
    makeMove(piece, location, white, current, new);

    //Now that new is set up right
    //If it's a pawn, then it's a promotion or en passant
    if (piece == W_P || piece == B_P)
    {
        //If in row 1 or row 8, then promoting pawn
        if ((location / 8) == 7 || (location / 8) == 0)
        {
            //piece code pointers
            uint8_t * p_codes = (white) ? new->w_codes : new->b_codes;

            //Update piece code
            p_codes[piece] = promote_to;
        }
        else
        {
            //opponent data pointers
            bitboard * op_all =
                    (white) ? &new->all_b_pieces : &new->all_w_pieces;
            //location bitboards
            bitboard * op_locs = (white) ? new->b_locations : new->w_locations;
            //piece value
            uint8_t * op_pcs = (white) ? new->b_pieces : new->w_pieces;

            //It's an en passant capture, so the pawn must be +- 1 row from
            //  location. It's +1 row if white, -1 row if black
            int8_t delta = (white) ? 8 : -8;
            //Get location of pawn being captured
            bitboard cap_loc = location_boards[location + delta];

            for (uint8_t i = 0; i < 16; ++i)
            {
                //If opponent piece at capture location
                if (op_locs[i] == cap_loc)
                {
                    //XOR out location in occupancy board
                    *op_all ^= op_locs[i];
                    //Set to invalid position
                    op_locs[i] = 0;
                    //Flag as captured
                    op_pcs[i] = CAPTURED;
                    //done
                    break;
                }
            }
        }
    }
    else
    {
        //Piece coming in will be a king (otherwise we won't know it's
        //  a castling maneuver per the project spec
        //King is in place because of makeMove, so need to determine rook to
        //  move and put it in place
        uint8_t rk;
        uint8_t rk_to;
        //Check for queenside vs kingside rook
        if ((location / 8) == 6)
        {
            //kingside rook, index is 09 of array
            rk = 9;
            //it goes to f1 (white) or f8 (black)
            //  These are -1 column back from king position (passed in)
            rk_to = location - 1;
        }
        else
        {
            //queenside rook, index is 08 of array
            rk = 8;
            //it goes to d1 (white) or d8 (black)
            //  These are +1 column back from king position (passed in)
            rk_to = location + 1;
        }
        //Use makeMove to put rook in right location
        makeMove(rk, rk_to, white, new, new);
    }
}

/*
 * Evaluates the value of a particular board
 *
 * @owner Daniel Rogers
 *
 * Based on:
 *   http://chessprogramming.wikispaces.com/Simplified+evaluation+function
 *
 * @uses all of the *_*_positions globals, modifier, piece_vals, *_codes
 *
 * @param board The board to evaluate
 * @return The value of the board in a form usable in a negamax function
 */
int evaluateState(chessboard * const board, bool white)
{
    int value = 0;
    int w_val = 0;
    int b_val = 0;

    //Sum up the values for white and black
    for (uint8_t i = 0; i < 16; ++i)
    {
        //Get each piece's value, and add in the value of its position
        //If it's captured, then add 0
        //White
        w_val +=
                (board->w_pieces[i] == CAPTURED) ?
                        0 :
                        piece_vals[board->w_codes[i]]
                                + board_position_vals[board->w_codes[i]][board->w_pieces[i]];
        //Same for Black
        b_val +=
                (board->b_pieces[i] == CAPTURED) ?
                        0 :
                        piece_vals[board->b_codes[i]]
                                + board_position_vals[board->b_codes[i]][board->b_pieces[i]];
    }

    //Value = white - black
    value = w_val - b_val;

    //If we are white, then we want max white, and this is already going to
    //  have higher scores for better values for white
    //If black, then multiply by -1 to flip it so higher scores returned mean
    //  better values for black
    value = (white) ? value : -value;

    return (value);
}

/*
 * Prints a board state
 *
 * @owner Daniel Rogers
 *
 * @param board The chessboard to print
 */
void printBoard(chessboard * const board)
{
    static const char cols[8] =
    { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    static const char rows[9] =
    { '1', '2', '3', '4', '5', '6', '7', '8', 'c' };

    char pos_str[3];
    pos_str[2] = '\0';

    puts("White:");
    for (uint8_t i = 0; i < 16; ++i)
    {
        pos_str[0] = cols[board->w_pieces[i] % 8];
        pos_str[1] = rows[board->w_pieces[i] / 8];

        printf("    pid: %d @ %s\n", board->w_codes[i], pos_str);
    }
    puts("Black:");
    for (uint8_t i = 0; i < 16; ++i)
    {
        pos_str[0] = cols[board->b_pieces[i] % 8];
        pos_str[1] = rows[board->b_pieces[i] / 8];
        printf("    pid: %d @ %s\n", board->b_codes[i], pos_str);
    }
}
