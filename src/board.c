/*
 * board.c
 * 
 * Implementations of the functions defined in board.h
 * 
 * @author Js
 * 
 */

#include "board.h"

/*
 * Initializes the board to its base state, where no moves have been made yet
 *
 * @uses white_initial, black_initial, location_bitboards, w_codes, b_codes
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

    //none of the rooks or the king has moved yet
    board->w_cancastle = KINGSIDE_ROOK | QUEENSIDE_ROOK;
    board->b_cancastle = KINGSIDE_ROOK | QUEENSIDE_ROOK;

    //No squares free between king and rooks
    board->w_castlefree = 0;
    board->b_castlefree = 0;

    //Initially no identical moves
    board->b_ident_moves = 0;
    board->w_ident_moves = 0;
    //No last piece moved
    board->w_last_piece = UINT8_MAX;
    board->b_last_piece = UINT8_MAX;
    //No position moved to
    board->w_last_move = INVALID_SQUARE;
    board->b_last_move = INVALID_SQUARE;
    board->w_last_move2 = INVALID_SQUARE;
    board->b_last_move2 = INVALID_SQUARE;

    //Set location arrays
    memcpy(board->w_piece_posns, white_initial, 16 * sizeof(uint8_t));
    memcpy(board->b_piece_posns, black_initial, 16 * sizeof(uint8_t));

    //Set initial piece codes
    memcpy(board->w_codes, w_codes, 16 * sizeof(uint8_t));
    memcpy(board->b_codes, b_codes, 16 * sizeof(uint8_t));

    //Set location bitboards
    for (uint8_t i = 0; i < 16; ++i)
    {
        board->w_locations[i] = location_boards[board->w_piece_posns[i]];
        board->b_locations[i] = location_boards[board->b_piece_posns[i]];
        //Update population bitboard
        board->all_w_pieces |= board->w_locations[i];
        board->all_b_pieces |= board->b_locations[i];
    }

#ifdef DEBUG_INIT
    for (uint8_t i = 0; i < 16; ++i)
    {
        fprintf(stdout, "w%d: %d\n", i, board->w_piece_posns[i]);
        fprintf(stdout, "wloc%d: %llx\n", i, board->w_locations[i]);
        fprintf(stdout, "b%d: %d\n", i, board->b_piece_posns[i]);
        fprintf(stdout, "bloc%d: %llx\n", i, board->b_locations[i]);
    }
    fprintf(stdout, "woc: %llx\n", board->all_w_pieces);
    fprintf(stdout, "boc: %llx\n", board->all_b_pieces);
#endif
}

/*
 * Expands the set of all possible board states from an initial state
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
    uint8_t * pieces = (white) ? board->w_piece_posns : board->b_piece_posns;
    //Side location occupancy boards
    bitboard self = (white) ? board->all_w_pieces : board->all_b_pieces;
    bitboard op = (white) ? board->all_b_pieces : board->all_w_pieces;
    //Lookup table piece codes
    uint8_t * codes = (white) ? board->w_codes : board->b_codes;

    //Number of states generated
    uint8_t states = 0;

    //Used to check for castling
    uint8_t cancastle, castlefree;
    uint8_t castleto;

    //The set of moves
    uint8_t (*moves)[7];

    bool capped;

    if (storage->count < 35)
    {
        //Allocate enough storage
        storage->data = realloc(storage->data, 35 * sizeof(chessboard));
        storage->count = 35;
    }

    //Loop variables
    uint8_t i = 15;
    uint8_t j, k;

    //For each piece, get the set of moves it can make from its location
    //  Traverse from king to pawns, because maybe this helps alphabeta
    do
    {
        //Check if piece is captured
        if (pieces[i] == CAPTURED)
        {
            continue;
        }

        assert(i < 16);
#ifdef DEBUG_MOVE
        fprintf(stdout, "index: %d\n", (int) i);
        fprintf(stdout, "piece: %d, @%d\n", codes[i], pieces[i]);
        fprintf(stdout, "move0: %d\n", legal_moves[codes[i]][pieces[i]][0][0]);
#endif

        moves = legal_moves[codes[i]][pieces[i]];

        //Go through each move ray
        for (j = 0; j < 8; ++j)
        {
            capped = false;
            //Go through each move in ray
            for (k = 0; k < 7; ++k)
            {
#ifdef DEBUG_MOVE
                fprintf(stdout, "making move: %d\n", moves[j][k]);
#endif
                if(invalidMoveSimple(location_boards[moves[j][k]], self, op,
                    codes[i], j == 0))
                {
                    //Stop looking through ray
#ifdef DEBUG_MOVE
                    puts("breaking");
#endif
                    break;
                }
                else
                {
                    //Expand special moves, do pawn promotion here as it's a
                    //  result of the move, rather than a unique move
                    if (((codes[i] == W_P) && ((moves[j][k] / 8) == 7))
                            || ((codes[i] == B_P) && ((moves[j][k] / 8) == 0)))
                    {
                        //pawn promotion, just make it a queen
                        moveSpecial(i, moves[j][k], white, board,
                                &storage->data[states++], (white) ? W_Q : B_Q);
                    }
                    else
                    {
                        //Make the move with the piece
                        capped = makeMove(i, moves[j][k], white, board,
                                &storage->data[states++]);
                    }

                    if (storage->count <= states)
                    {
                        //Allocate more storage
                        storage->count += 10;
                        storage->data = realloc(storage->data,
                                storage->count * sizeof(chessboard));
                    }

                    //See if this was a capturing move, and stop moving along
                    //  ray if so
                    if (capped)
                    {
                        break;
                    }
                }
                //End making legal moves in ray
            }
            //End piece ray traversals
            //do castling here
            if ((codes[i] == W_K && board->w_cancastle && board->w_castlefree)
                    || (codes[i] == B_K && board->b_cancastle
                            && board->b_castlefree))
            {
                //Check if squares matching unoccupied space are free
                cancastle = (white) ? board->w_cancastle : board->b_cancastle;
                castlefree =
                        (white) ? board->w_castlefree : board->b_castlefree;

                if ((cancastle & KINGSIDE_ROOK)
                        && ((castlefree & KINGSIDE_FREE) == KINGSIDE_FREE))
                {
                    castleto = (white) ? KINGSIDE_W_CASTLE : KINGSIDE_B_CASTLE;
                    //Do the castling, king to g1 or g1
                    moveSpecial(i, castleto, white, board,
                            &storage->data[states++], 0);
                    if (storage->count <= states)
                    {
                        //Allocate more storage
                        storage->count += 10;
                        storage->data = realloc(storage->data,
                                storage->count * sizeof(chessboard));
                    }
#ifdef DEBUG_MOVE
                    fprintf(stdout, "castled kingside: %d, %x\n", cancastle, castlefree);
                    printBoard(&storage->data[states-1]);
#endif
                }
                if ((cancastle & QUEENSIDE_ROOK)
                        && ((castlefree & QUEENSIDE_FREE) == QUEENSIDE_FREE))
                {
                    castleto =
                            (white) ? QUEENSIDE_W_CASTLE : QUEENSIDE_B_CASTLE;
                    moveSpecial(i, castleto, white, board,
                            &storage->data[states++], 0);
                    if (storage->count <= states)
                    {
                        //Allocate more storage
                        storage->count += 10;
                        storage->data = realloc(storage->data,
                                storage->count * sizeof(chessboard));
                    }
#ifdef DEBUG_MOVE
                    fprintf(stdout, "castled queenside: %d, %x\n", cancastle,
                            castlefree);
                    printBoard(&storage->data[states-1]);
#endif
                }
            }
        }
    } while(i--);

    return (states);
}

/**
 * Makes simple checks against a destination square to validate moves
 *
 * @param destination The bitboard representing the destination of a piece
 * @param self_pieces A bitboard representing the locations of all pieces
 *          belonging to the side (w|b) making the move
 * @param opponent_pieces A bitboard representing the lcoations of all pieces
 *          belonging to the opposing side
 * @param piece_code The piece moving
 * @param movingForward should be true if the piece is moving forward from its
 * perspective
 *
 * @return true if the destination square does not contain an allied piece, or
 * if the piece is a pawn and if moving forward, no opposing piece is at the
 * destination, or if moving diagonally, an opposing piece is at the destination
 */
bool invalidMoveSimple(bitboard destination, bitboard self_pieces,
        bitboard opponent_pieces, uint8_t piece_code, bool movingForward)
{
    //Check for end of ray, or own piece @ location
    //Location check works as follows:
    //  Get the bitboard representing the destination
    //  Then AND with pieces for own side -> all 0 if no pieces at
    //      destination, meaning valid move. !0 if own piece at
    //      location, meaning invalid move
    //location_boards[64] == 0xffffffffffffffff, so any AND will
    //  make a non-0 value
    return (destination & self_pieces) ||
        (
            (piece_code == W_P || piece_code == B_P) &&
            /*!! NOT the result, since the XOR check is for a VALID move */
            ! (
                //if moving forward, must have opponent piece
                //  if moving forward, must NOT have opponent piece
                movingForward ^ (destination & opponent_pieces)
            )
        );
}

/*
 * Generates a new board state based on a piece move
 *
 * @param pindex The index of the piece to move
 * @param location The location to move to
 * @param white true If the piece being moved is white
 * @param current The chessboard state being referenced
 * @param new The new chessboard state to write to
 */
bool makeMove(uint8_t pindex, uint8_t location, bool white,
        chessboard * const current, chessboard * new)
{
    //Generate the new location bitboard for the new location
    bitboard new_loc = location_boards[location];

    //copy data
    memmove(new, current, sizeof(chessboard));

    uint8_t * ident_moves = (white) ? &new->w_ident_moves : &new->b_ident_moves;
    uint8_t * last_move = (white) ? &new->w_last_move : &new->b_last_move;
    uint8_t * last_move2 = (white) ? &new->w_last_move2 : &new->b_last_move2;
    uint8_t * last_piece = (white) ? &new->w_last_piece : &new->b_last_piece;

    //Update last move
    //See if moved to the same location last or two turns ago
    //  if so, update ident count
    *ident_moves =
            (*last_move == location || *last_move2 == location) ?
                    (*ident_moves) + 1 : 0;
    *last_move2 = *last_move;
    *last_move = location;
    *last_piece = pindex;

    //Set up data pointers
    //All pieces
    bitboard * self_all = (white) ? &new->all_w_pieces : &new->all_b_pieces;
    bitboard * op_all = (white) ? &new->all_b_pieces : &new->all_w_pieces;
    //location bitboards
    bitboard * self_locs = (white) ? new->w_locations : new->b_locations;
    bitboard * op_locs = (white) ? new->b_locations : new->w_locations;
    //piece value
    uint8_t * self_pcs = (white) ? new->w_piece_posns : new->b_piece_posns;
    uint8_t * op_pcs = (white) ? new->b_piece_posns : new->w_piece_posns;

    //Castling data
    uint8_t * cancastle = (white) ? &new->w_cancastle : &new->b_cancastle;

    uint8_t * op_cancastle = (white) ? &new->b_cancastle : &new->w_cancastle;

    //Update appropriately for castling
    if (*cancastle)
    {
        if (pindex == 15 || pindex == 8 || pindex == 9)
        {
            //king or rook moved
            switch (pindex)
            {
            case 15:
                //king moved
                *cancastle = 0;
                break;
            case 8:
                //queenside rook moved
                *cancastle &= KINGSIDE_ROOK;
                break;
            case 9:
                //kingside rook moved
                *cancastle &= QUEENSIDE_ROOK;
                break;
            }
        }

        if ((self_pcs[pindex] / 8) == 0)
        {
            //Moving out of row 1 opening spot for castling
            //Turn on bit at location
            new->w_castlefree |= (1 << (self_pcs[pindex] % 8));
        }
        else if ((self_pcs[pindex] / 8) == 7)
        {
            //Moving out of row 8 opening spot for castling
            //Turn on bit at location
            new->b_castlefree |= (1 << (self_pcs[pindex] % 8));
        }

        if ((location / 8) == 0)
        {
            //Moving into row 1 closing spot for castling
            //Turn off bit at location
            new->w_castlefree &= ~(1 << (location % 8));
        }
        else if ((location / 8) == 7)
        {
            //Moving into row 8 closing spot for castling
            //Turn off bit at location
            new->b_castlefree &= ~(1 << (location % 8));
        }
    }

    //Update the occupancy bitboard
    //XOR out the old location, XOR in the new location
    *self_all ^= (self_locs[pindex] ^ new_loc);
    //Update position bitboard
    self_locs[pindex] = new_loc;
    //update piece location
    self_pcs[pindex] = location;

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

                //See if ability for opponent to castle has changed
                if (*op_cancastle && (i == 15 || i == 8 || i == 9))
                {
                    //king or rook moved
                    switch (pindex)
                    {
                    case 15:
                        //king moved
                        *op_cancastle = 0;
                        break;
                    case 8:
                        //queenside rook capped
                        *op_cancastle &= KINGSIDE_ROOK;
                        break;
                    case 9:
                        //kingside rook capped
                        *op_cancastle &= QUEENSIDE_ROOK;
                        break;
                    }
                }
                //done
                break;
            }
        }

        return (true);
    }

    return (false);
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
 * @param pindex The index of the piece to move
 * @param location The location to move to
 * @param white true If the piece being moved is white
 * @param current The chessboard state being referenced
 * @param new The new chessboard state to write to
 * @param promote_to If promoting, this is the piece code of the desired
 *                   promotion
 */
void moveSpecial(uint8_t pindex, uint8_t location, bool white,
        chessboard * const current, chessboard * new, uint8_t promote_to)
{

    //Now that new is set up right
    //If it's a pawn, then it's a promotion or en passant
    if ((white && current->w_codes[pindex] == W_P)
            || (!white && current->b_codes[pindex] == B_P))
    {
        //do initial work with makeMove
        //Capturing during a pawn promotion move will be handled by this
        makeMove(pindex, location, white, current, new);

        //If in row 1 or row 8, then promoting pawn
        if ((location / 8) == 7 || (location / 8) == 0)
        {
            //piece code pointers
            uint8_t * p_codes = (white) ? new->w_codes : new->b_codes;

            //Update piece code
            p_codes[pindex] = promote_to;
        }
        else
        {
            //opponent data pointers
            bitboard * op_all =
                    (white) ? &new->all_b_pieces : &new->all_w_pieces;
            //location bitboards
            bitboard * op_locs = (white) ? new->b_locations : new->w_locations;
            //piece value
            uint8_t * op_pcs = (white) ? new->b_piece_posns : new->w_piece_posns;

            //It's an en passant capture, so the pawn must be +- 1 row from
            //  location. It's -1 row if white, +1 row if black
            int8_t delta = (white) ? -8 : 8;
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
        if ((location % 8) == 6)
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
        //Use makeMove to put rook into right location & copy to new
        makeMove(rk, rk_to, white, current, new);
        //Move King into right location and update new
        makeMove(pindex, location, white, new, new);
    }
}

/*
 * Evaluates the value of a particular board
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
    //TODO Maybe this can be more complex? expansion is pretty fast
    int value = 0;
    int w_val = 0;
    int b_val = 0;

    if (board->w_piece_posns[15] != CAPTURED && board->b_piece_posns[15] != CAPTURED
            && (board->w_ident_moves >= 3 || board->b_ident_moves >= 3))
    {
        return (0);
    }

    //Check for endgame state
    if (board->w_piece_posns[14] == CAPTURED && board->b_piece_posns[14] == CAPTURED)
    {
        board_position_vals[11] = b_K_e_positions;
    }
    else
    {
        board_position_vals[11] = b_K_m_positions;
    }

    //Sum up the values for white and black
    for (uint8_t i = 0; i < 16; ++i)
    {
        //Get each piece's value, and add in the value of its position
        //If it's captured, then add 0
        //White
        w_val +=
                (board->w_piece_posns[i] == CAPTURED) ?
                        0 :
                        piece_vals[board->w_codes[i]]
                                + board_position_vals[board->w_codes[i]][board->w_piece_posns[i]];
        //Same for Black
        b_val +=
                (board->b_piece_posns[i] == CAPTURED) ?
                        0 :
                        piece_vals[board->b_codes[i]]
                                + board_position_vals[board->b_codes[i]][board->b_piece_posns[i]];
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
 * @param board The chessboard to print
 */
void printBoard(chessboard * const board)
{
    char pos_str[3];

    puts("White:");
    for (uint8_t i = 0; i < 16; ++i)
    {
        squareToString(board->w_piece_posns[i], pos_str);

        printf("    pid: %d @ %s\n", board->w_codes[i], pos_str);
    }
    //printf("    occupancy: %llx\n", board->all_w_pieces);

    puts("Black:");
    for (uint8_t i = 0; i < 16; ++i)
    {
        squareToString(board->b_piece_posns[i], pos_str);
        printf("    pid: %d @ %s\n", board->b_codes[i], pos_str);
    }
    //printf("    occupancy: %llx\n", board->all_b_pieces);
}

/*
 * Gets a movestring in the right format for the server
 *
 * @param board The board to generate a movestring for
 * @param prev The board state prior to board
 * @param white True if generating a movestring for white
 * @param out An array of char[7] to fill with the movestring
 */
void getMoveString(chessboard * const board, chessboard * const prev,
bool white, char out[7])
{
    //The piece that moved
    uint8_t piece = (white) ? board->w_last_piece : board->b_last_piece;
    //The location it was in
    uint8_t last_loc = (white) ? prev->w_piece_posns[piece] : prev->b_piece_posns[piece];
    //The location it moved to
    uint8_t last_mv = (white) ? board->w_last_move : board->b_last_move;
    //The piece ID
    uint8_t pid = (white) ? prev->w_codes[piece] : prev->b_codes[piece];

    //First get the piece code
    out[0] = piece_chars[pid];
    //then the square from, into 1,2 (3 will be \0)
    squareToString(last_loc, &out[1]);
    //And the square to, into 3, 4 (5 will be \0)
    squareToString(last_mv, &out[3]);

    if (pid == W_P || pid == B_P)
    {
        //Check for pawn promotion
        uint8_t npid = (white) ? board->w_codes[piece] : prev->b_codes[piece];
        //Record promotion (if applicable)
        out[5] = (npid != pid) ? piece_chars[npid] : out[5];
    }
    out[6] = '\0';
}

/**
 * Parses a move string and makes the appropriate move
 *
 * @param move The move string to process
 * @param white true if the move was made by white
 * @param board The board to update
 */
void parseMoveString(char move[7], bool white, chessboard * board)
{
    //Get piece character
    char piece = move[0];
    //Get data for start position
    char col_start = move[1];
    uint8_t row_start = (uint8_t) atoi(&move[2]) - 1;

#ifdef DEBUG
    fprintf(stdout, "cs: %c, rs: %d\n", col_start, row_start);
#endif

    //Get data for end position
    char col_end = move[3];
    uint8_t row_end = (uint8_t) atoi(&move[4]) - 1;

#ifdef DEBUG
    fprintf(stdout, "ce: %c, re: %d\n", col_end, row_end);
#endif

    //Get data for promotion
    char promote = move[5];

    //Compute square on board for start
    uint8_t square_start = row_start * 8;
    //shhhhhh
    square_start += (col_start - 'a');
#ifdef DEBUG
    fprintf(stdout, "ss: %d\n", square_start);
#endif

    //Compute square on board for end
    uint8_t square_end = row_end * 8;
#ifdef DEBUG
    fprintf(stdout, "seinitial: %d\n", square_end);
#endif
    //shhhhhh
    square_end += (col_end - 'a');

    //Find the array index
    uint8_t * pieces = (white) ? board->w_piece_posns : board->b_piece_posns;
    uint8_t pindex;

    bitboard op_oc = (white) ? board->all_b_pieces : board->all_w_pieces;

    //Find matching location
    for (pindex = 0; pindex < 16; ++pindex)
    {
        if (pieces[pindex] == square_start)
        {
            break;
        }
    }

#ifdef DEBUG
    fprintf(stdout, "making move [piece, dest, white]: %d, %d, %d\n", pindex, square_end,
            white);
#endif

    //See if it's a promotion
    if (promote != '\0')
    {
#ifdef DEBUG
        fprintf(stdout, "promotion to: %c\n", promote);
#endif

        uint8_t promote_to = (white) ? 0 : 6;
        //Get the piece code
        for (uint8_t i = 0; i < 6; ++i, ++promote_to)
        {
            if (promote == piece_chars[i])
            {
                break;
            }
        }

        //Do the promotion
        moveSpecial(pindex, square_end, white, board, board, promote_to);
    }
    //Pawn move, check for en passant
    //  If moving diagonally, and not a piece @ location
    else if ((piece == 'P') && (col_start != col_end)
            && !(location_boards[square_end] & op_oc))
    {
#ifdef DEBUG
        puts("en passant");
#endif
        moveSpecial(pindex, square_end, white, board, board, 0);
    }
    //King move
    //Starting in start column
    //Staying in same row
    //moving more than one column
    //Must be a castling maneuver
    else if (piece == 'K' && (col_start == 'e')
            && (col_end != 'd' && col_end != 'f') && (row_start == row_end))
    {
#ifdef DEBUG
        puts("castle");
#endif
        moveSpecial(pindex, square_end, white, board, board, 0);
    }
    //Normal move
    else
    {
        makeMove(pindex, square_end, white, board, board);
    }
}

/*
 * Converts a board coordinate to a notation string
 *
 * @param pos The position (0-64) of the piece
 * @param str An array of 3 characters to fill, string[2] will be null
 *                  - ac indicates a capture
 */
void squareToString(uint8_t pos, char str[3])
{
    static const char cols[8] =
    { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    static const char rows[9] =
    { '1', '2', '3', '4', '5', '6', '7', '8', 'c' };
    str[2] = '\0';

    str[0] = cols[pos % 8];
    str[1] = rows[pos / 8];
}

/*
 * for readability/debugging purposes, should not be used for net moves
 *
 * @param pos Where in the array it came from
 * @param piece_code the piece code
 * @param str The string to write to, will be color|piece|#
 */
void pieceToString(uint8_t pos, uint8_t piece_code, char str[4])
{
    str[3] = '\0';
    str[0] = (piece_code < 6) ? 'w' : 'b';
    str[1] = piece_chars[piece_code];
    str[2] = '1';
    str[2] += (piece_code == 6 || piece_code == 0) ? pos % 8 : pos % 2;
}
