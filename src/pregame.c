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
 * @owner Daniel Rogers
 *
 * @modifies legal_moves, attacked_squares in board.h
 */
void generateMoveTables()
{
    for (uint8_t i = 0; i < 12; ++i)
    {
        //Set all the bitboards to 0 initially
        memset(attacked_squares[i], 0, 64 * sizeof(bitboard));
        //Set all the legal moves to INVALID_SQUARE initially
        memset(legal_moves[i], INVALID_SQUARE, 64 * 8 * 7 * sizeof(uint8_t));
    }

    //Loop through the squares and generate each set of moves for the squares
    //  Technically there are duplicates/overlapping values in here, but space
    //  is very cheap for this, and this will make it simpler and faster to
    //  look up stuff later. This is also done BEFORE the game, so time spent
    //  here is unimportant (so long as it's feasible)
    for (uint8_t i = 0; i < 64; ++i)
    {
        //Generate white moves
        //pawns
        calcPawnMoves(i, legal_moves[W_P][i], attacked_squares[W_P][i], true);
        //bishops
        calcBishopMoves(i, legal_moves[W_B][i], attacked_squares[W_B][i]);
        //knights
        calcKnightMoves(i, legal_moves[W_N][i], attacked_squares[W_N][i]);
        //rooks
        calcRookMoves(i, legal_moves[W_R][i], attacked_squares[W_R][i]);
        //queen
        calcQueenMoves(i, legal_moves[W_Q][i], attacked_squares[W_Q][i]);
        //king
        calcKingMoves(i, legal_moves[W_K][i], attacked_squares[W_K][i]);

        //Generate black moves
        //pawns
        calcPawnMoves(i, legal_moves[B_P][i], attacked_squares[B_P][i], false);
        //bishops
        calcBishopMoves(i, legal_moves[B_B][i], attacked_squares[B_B][i]);
        //knights
        calcKnightMoves(i, legal_moves[B_N][i], attacked_squares[B_N][i]);
        //rooks
        calcRookMoves(i, legal_moves[B_R][i], attacked_squares[B_R][i]);
        //queen
        calcQueenMoves(i, legal_moves[B_Q][i], attacked_squares[B_Q][i]);
        //king
        calcKingMoves(i, legal_moves[B_K][i], attacked_squares[B_K][i]);
    }

    //Table files in local directory
    FILE * move_table = fopen("move_table.bin", "wb");
    FILE * atk_table = fopen("atk_boards.bin", "wb");

    if (!move_table || !atk_table)
    {
        puts("Unable to write table files. Work will not be saved!");
    }
    else
    {
        //write files
        //12 different pieces, 64 squares, 8 directions, 7 max moves/direction
        fwrite(legal_moves, sizeof(uint8_t), 12 * 64 * 8 * 7, move_table);
        //12 different pieces, 64 squares
        fwrite(attacked_squares, sizeof(bitboard), 12 * 64, atk_table);

        fclose(move_table);
        fclose(atk_table);

        puts("Table files generated successfully");
    }
}

/*
 * Loads in a binary file created by generateMoveTable to initializer the
 * pre-calculated move tables.
 *
 * @owner Daniel Rogers
 *
 * @modifies legal_moves, attacked_squares in board.h
 *
 * @returns true if the file loaded successfully, false if some error occurred
 *          which prevented a successful initialization of legal_moves and
 *          attacked_squares.
 */
bool loadMoveTables()
{
    //Table files in local directory
    FILE * move_table = fopen("move_table.bin", "rb");
    FILE * atk_table = fopen("atk_boards.bin", "rb");

    if (!move_table || !atk_table)
    {
        puts("Unable to open table files for reading...");
        return (false);
    }
    else
    {
        //read files
        //12 different pieces, 64 squares, 8 directions, 7 max moves/direction
        fread(legal_moves, sizeof(uint8_t), 12 * 64 * 8 * 7, move_table);
        //12 different pieces, 64 squares
        fread(attacked_squares, sizeof(bitboard), 12 * 64, atk_table);

        fclose(move_table);
        fclose(atk_table);
        puts("Table files loaded successfully");
        return (true);
    }
}

/*
 * Populates the hashkey table for the board with randomly generated N-bit
 * values, where N is the bitwidth of HASHKEY.
 *
 * @modifies key_table in board.h
 */
void generateHashkeys()
{
    //TODO Generate the hashkey tables
}

/*
 * Calculates the moves available to a pawn piece from a location
 *
 * @owner Daniel Rogers
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
bool white)
{
    int8_t delta = (white) ? 8 : -8;
    //If location / 8 == 7, then it's a value in the range 56-63
    //  and in the last row, so no moves are valid for white (except promotion)
    //  Same if location / 8 == 0 and black
    //White starts in row two, which is >= 8, black starts in row 7
    //  which is < 56
    if (((location / 8) > 0) && ((location / 8) < 7))
    {
        //white moves away from 0:0, black moves towards 0:0
        uint8_t col = location % 8;
        //Valid moves are up and left, up, up and right
        //up
        moves[0][0] = location + delta;

        //Check for right edge, if location %8 == 7, then it's a rightmost square
        //Update the attack bitboard, pawns can only cap diagonally
        // This does not account for en passant captures
        moves[1][0] = (col == 7) ? INVALID_SQUARE : (moves[0][0] + 1);
        atkbboard |= ON << (moves[1][0]);

        //Check for left edge, if location % 8 == 0, then it's a leftmost square
        //  and can have no up/left value, otherwise move up a row and back 1
        moves[7][0] = (col == 0) ? INVALID_SQUARE : (moves[0][0] - 1);
        atkbboard |= ON << (moves[7][0]);
    }
    //Pawns can move 2 moves from start position, so account for special case
    if ((white && ((location / 8) == 1)) || (!white && ((location / 8) == 6)))
    {
        //up 2
        moves[0][1] = location + 2 * delta;
    }
}

/*
 * Calculates the moves available to a knight piece from a location
 *
 * @owner Daniel Rogers
 *
 * @param location The location of the knight
 * @param moves An array to fill with the available moves from that location
 *              They will travel clockwise from the top-right
 *              (from white's perspective)
 *              Assumes that it is preset to INVALID
 * @param atk_bboard The attack bitboard to configure for the location
 *                      It is assumed to be set to 0.
 *                      Does not account for en passant captures
 */
void calcKnightMoves(uint8_t location, uint8_t moves[8][7], bitboard atkbboard)
{
    //Check if location allows movement upwards, and how much
    //  Needed for up2/right, right2/up, up2/left, left2/up
    //  if location is < 7 * 8, then able to move upwards at least 1
    if (location / 8 < 7)
    {
        //Needed for up2/right, up2/left
        //  if location is < 6 * 8, then able to move upwards at least 2
        if (location / 8 < 6)
        {
            //if location % 8 < 7, then can move one right
            //  Knight moves 2 up, 1 over, so set to 2*8 + 1 for + 2 rows + 1 col
            moves[0][0] =
                    (location % 8 > 6) ? INVALID_SQUARE : location + 16 + 1;
            //  if location % 8 > 0, then can move one left
            moves[7][0] =
                    (location % 8 == 0) ? INVALID_SQUARE : location + 16 - 1;
        }

        //If location % 8 < 6, then can move 2 right and up 1
        moves[1][0] = (location % 8 > 5) ? INVALID_SQUARE : location + 2 + 8;
        //if location % 8 > 1, then can move 2 left and up 1
        moves[6][0] = (location % 8 < 2) ? INVALID_SQUARE : location - 2 + 8;
    }

    //Check for movement downwards and how much
    //  Needed for right2/down, down2/right, down2/left, left2/down
    if (location / 8 > 0)
    {
        //Needed for down2/right, down2/left
        //  if location / 8 > 1, then able to move down at least 2
        if (location / 8 > 1)
        {
            //if location % 8 < 7, then able to move right at least 1
            moves[3][0] =
                    (location % 8 > 6) ? INVALID_SQUARE : location - 16 + 1;
            //if location % 8 > 0, then able to move left at least 1
            moves[4][0] = (location % 8 == 0) ?
            INVALID_SQUARE :
                                                location - 16 - 1;
        }

        //if location % 8 < 6, then able to move right at least 2
        moves[2][0] = (location % 8 > 5) ? INVALID_SQUARE : location + 2 - 8;
        //if location % 8 > 1, then able to move left at least 2
        moves[5][0] = (location % 8 < 2) ? INVALID_SQUARE : location - 2 - 8;
    }

    //update the attack bitboard
    for (uint8_t i = 0; i < 8; ++i)
    {
        atkbboard =
                (moves[i][0] != INVALID_SQUARE) ?
                        atkbboard | (ON << moves[i][0]) : atkbboard;
    }
}

/*
 * Calculates the moves available to a bishop piece from a location
 *
 * @owner Daniel Rogers
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
 *                      Does not account for en passant captures
 */
void calcBishopMoves(uint8_t location, uint8_t moves[4][7], bitboard atkbboard)
{
    //This is the same as for the queen, but without the
    //  vertical/horizontal moves
    uint8_t NE, SE, SW, NW;
    NE = SE = SW = NW = location;

    //Extend rays
    for (uint8_t i = 0; i < 7; ++i)
    {
        moves[0][i] = (NE < INVALID_SQUARE) ? NE : INVALID_SQUARE;
        moves[1][i] = (SE < INVALID_SQUARE) ? SE : INVALID_SQUARE;
        moves[2][i] = (SW < INVALID_SQUARE) ? SW : INVALID_SQUARE;
        moves[3][i] = (NW < INVALID_SQUARE) ? NW : INVALID_SQUARE;

        //Update attack bitboards
        atkbboard =
                (NE < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[0][i]) : atkbboard;
        atkbboard =
                (SE < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[1][i]) : atkbboard;
        atkbboard =
                (SW < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[2][i]) : atkbboard;
        atkbboard =
                (NW < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[3][i]) : atkbboard;

        //Update ray positions on board
        //Uses the fact that unsigned integer overflow has defined behavior
        //Moving up a row and right a column
        //  Don't go off the right edge of the board
        NE = (NE % 8 < 7 && NE < INVALID_SQUARE) ? NE + 8 + 1 : INVALID_SQUARE;
        //Moving down a row and right a column
        SE = (SE % 8 < 7 && SE < INVALID_SQUARE) ? SE - 8 + 1 : INVALID_SQUARE;
        //Moving down a row and left a column
        SW = (SW % 8 > 0 && SW < INVALID_SQUARE) ? SW - 8 - 1 : INVALID_SQUARE;
        //Moving up a row and left a column
        NW = (NW % 8 > 0 && NW < INVALID_SQUARE) ? NW + 8 - 1 : INVALID_SQUARE;
    }
}

/*
 * Calculates the moves available to a rook piece from a location
 *
 * @owner Daniel Rogers
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
 *                      Does not account for en passant captures
 */
void calcRookMoves(uint8_t location, uint8_t moves[4][7], bitboard atkbboard)
{
    //Same as for queen, but only horizontal/vertical
    uint8_t N, E, S, W;
    N = E = S = W = location;

    //Extend rays
    for (uint8_t i = 0; i < 7; ++i)
    {
        moves[0][i] = (N < INVALID_SQUARE) ? N : INVALID_SQUARE;
        moves[1][i] = (E < INVALID_SQUARE) ? E : INVALID_SQUARE;
        moves[2][i] = (S < INVALID_SQUARE) ? S : INVALID_SQUARE;
        moves[3][i] = (W < INVALID_SQUARE) ? W : INVALID_SQUARE;

        //Update attack bitboards
        atkbboard =
                (N < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[0][i]) : atkbboard;
        atkbboard =
                (E < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[1][i]) : atkbboard;
        atkbboard =
                (S < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[2][i]) : atkbboard;
        atkbboard =
                (W < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[3][i]) : atkbboard;

        //Update ray positions on board
        //Moving up a row
        N = (N < INVALID_SQUARE) ? N + 8 : INVALID_SQUARE;
        //Moving right a column
        E = (E % 8 < 7 && E < INVALID_SQUARE) ? E + 1 : INVALID_SQUARE;
        //Moving down a row
        S = (S < INVALID_SQUARE) ? S - 8 : INVALID_SQUARE;
        //Moving left a column
        W = (W % 8 > 0 && W < INVALID_SQUARE) ? W - 1 : INVALID_SQUARE;
    }
}

/*
 * Calculates the moves available to a queen piece from a location
 *
 * @owner Daniel Rogers
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
 *                      Does not account for en passant captures
 */
void calcQueenMoves(uint8_t location, uint8_t moves[8][7], bitboard atkbboard)
{
    uint8_t N, NE, E, SE, S, SW, W, NW;
    N = NE = E = SE = S = SW = W = NW = location;

    //Extend rays
    for (uint8_t i = 0; i < 7; ++i)
    {
        moves[0][i] = (N < INVALID_SQUARE) ? N : INVALID_SQUARE;
        moves[1][i] = (NE < INVALID_SQUARE) ? NE : INVALID_SQUARE;
        moves[2][i] = (E < INVALID_SQUARE) ? E : INVALID_SQUARE;
        moves[3][i] = (SE < INVALID_SQUARE) ? SE : INVALID_SQUARE;
        moves[4][i] = (S < INVALID_SQUARE) ? S : INVALID_SQUARE;
        moves[5][i] = (SW < INVALID_SQUARE) ? SW : INVALID_SQUARE;
        moves[6][i] = (W < INVALID_SQUARE) ? W : INVALID_SQUARE;
        moves[7][i] = (NW < INVALID_SQUARE) ? NW : INVALID_SQUARE;

        //Update attack bitboards
        atkbboard =
                (N < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[0][i]) : atkbboard;
        atkbboard =
                (NE < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[1][i]) : atkbboard;
        atkbboard =
                (E < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[2][i]) : atkbboard;
        atkbboard =
                (SE < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[3][i]) : atkbboard;
        atkbboard =
                (S < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[4][i]) : atkbboard;
        atkbboard =
                (SW < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[5][i]) : atkbboard;
        atkbboard =
                (W < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[6][i]) : atkbboard;
        atkbboard =
                (NW < INVALID_SQUARE) ?
                        atkbboard | (ON << moves[7][i]) : atkbboard;

        //Update ray positions on board
        //Moving up a row
        N = (N < INVALID_SQUARE) ? N + 8 : INVALID_SQUARE;
        //Moving up a row and right a column
        //  Don't go off the right edge of the board
        NE = (NE % 8 < 7 && NE < INVALID_SQUARE) ? NE + 8 + 1 : INVALID_SQUARE;
        //Moving right a column
        E = (E % 8 < 7 && E < INVALID_SQUARE) ? E + 1 : INVALID_SQUARE;
        //Moving down a row and right a column
        //  Since using unsigned integers
        //  underflow (which has defined behavior) will cause the value to
        //      be > 64 (INVALID_SQUARE)
        SE = (SE % 8 < 7 && SE < INVALID_SQUARE) ? SE - 8 + 1 : INVALID_SQUARE;
        //Moving down a row
        S = (S < INVALID_SQUARE) ? S - 8 : INVALID_SQUARE;
        //Moving down a row and left a column
        SW = (SW % 8 > 0 && SW < INVALID_SQUARE) ? SW - 8 - 1 : INVALID_SQUARE;
        //Moving left a column
        W = (W % 8 > 0 && W < INVALID_SQUARE) ? W - 1 : INVALID_SQUARE;
        //Moving up a row and left a column
        NW = (NW % 8 > 0 && NW < INVALID_SQUARE) ? NW + 8 - 1 : INVALID_SQUARE;
    }
}

/*
 * Calculates the moves available to a king from a location
 *
 * @owner Daniel Rogers
 *
 * @param location The location of the king
 * @param moves An array to fill with the available moves from that location.
 *              Each array will be filled with the moves available to the
 *              piece, traveling clockwise from the top
 *              (from white's perspective)
 *              Assumes that it is preset to INVALID
 * @param atk_bboard The attack bitboard to configure for the location
 *                      It is assumed to be set to 0.
 *                      Does not account for en passant captures
 */
void calcKingMoves(uint8_t location, uint8_t moves[8][7], bitboard atkbboard)
{
    bool can_up = (location / 8 < 7);
    bool can_down = (location / 8 > 0);
    bool can_right = (location % 8 < 7);
    bool can_left = (location % 8 > 0);

    //Move up 1
    moves[0][0] = (can_up) ? location + 8 : INVALID_SQUARE;
    //Move up 1 and right 1
    moves[1][0] = (can_up && can_right) ? location + 8 + 1 : INVALID_SQUARE;
    //Move right 1
    moves[2][0] = (can_right) ? location + 1 : INVALID_SQUARE;
    //Move right 1 and down 1
    moves[3][0] = (can_right && can_down) ? location + 1 - 8 : INVALID_SQUARE;
    //Move down 1
    moves[4][0] = (can_down) ? location - 8 : INVALID_SQUARE;
    //Move down 1 and left 1
    moves[5][0] = (can_down && can_left) ? location - 8 - 1 : INVALID_SQUARE;
    //Move left 1
    moves[6][0] = (can_left) ? location - 1 : INVALID_SQUARE;
    //Move left 1 and up 1
    moves[7][0] = (can_left && can_up) ? location - 1 + 8 : INVALID_SQUARE;

    for (uint8_t i = 0; i < 8; ++i)
    {
        atkbboard =
                (moves[i][0] != INVALID_SQUARE) ?
                        atkbboard | (ON < moves[i][0]) : atkbboard;
    }
}

