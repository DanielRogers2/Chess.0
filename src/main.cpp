#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "net.h"
#include "json.h"
#include "reader.h"
#include "writer.h"
#include "value.h"
#include <sstream>


extern "C" {
#include <sys/time.h>
    
#include "board.h"
#include "pregame.h"
#include "brain.h"
}

std::string poll(std::string gameId, std::string teamNumSec);
int getPiece(int location, std::string color, chessboard* board);
int getLocation(std::string location);

int main() {
    using namespace std;
    string gameId;
    cout << "Enter game Id: ";
    cin >> gameId;
    string teamNum;
    cout << "Enter team number: ";
    cin >> teamNum;
    string teamSecret;
    cout << "Enter team secret: ";
    cin >> teamSecret;
    string color;
    cout << "Enter color: ";
    cin >> color;
    
    //Load or generate tables
    if (!loadMoveTables())
    {
        //No tables available, generate them
        generateMoveTables();
    }
    
#ifdef DEBUG_TABLES
    //printf("addr_LM: %p\n", &legal_moves);
    //printf("move00: %d\n", legal_moves[0][0][0][0]);
#endif
    
    //Get a new board and initialize it
    chessboard current_state;
    initBoard(&current_state);
    chessboard res;
    
    boardset newstates;
    newstates.count = 0;
    newstates.data = NULL;
    
    uint8_t exp = expandStates(&current_state, &newstates, true);
    
    //printf("expanded: %u\n", exp);
    //printf("%llx\n", newstates.data[0].all_w_pieces);
    
    //Timers
    clock_t tstart, tend;
    double tex;
    
    //15 mins each
    double twhite = 900;
    double tblack = 900;
    
    //Record a play string to allow animation if desired
    char plays[256][7] =
    {
        { 0 } };
    
    
    
    bool white_won, draw;
    draw = white_won = false;
    
    uint8_t counter = 0;
    
    uint8_t w_ply = 7;
    uint8_t b_ply = 7;
    
    
    bool playing = true;
    while(playing) {
        // white
        if(color == "white") {
            tstart = clock();
            tend = clock();
            
            std::string opponentMove = poll(gameId, teamNum + "/" + teamSecret);
            cout << opponentMove << endl;
            if(opponentMove != "") {
                
                std::string pieceStr = opponentMove.substr(1, 2);
                std::string locationStr = opponentMove.substr(3, 2);
                int piece = getLocation(pieceStr);
                piece = getPiece(piece, "black", &current_state);
                int location = getLocation(locationStr);
                
                if(opponentMove == "Ke1g1" | opponentMove == "Ke1c1" | opponentMove == "Ke8g8" | opponentMove == "Ke8c8" | opponentMove.length() == 6) {
                    moveSpecial(piece, location, false, &current_state, &res, (false) ? W_Q : B_Q);
                    current_state = res;
                } else {
                    makeMove(piece, location, false, &current_state, &res);
                    current_state = res;
                }
            }
            
            selectBestMove(true, &current_state, &res, w_ply, twhite);
            
            getMoveString(&res, &current_state, true, plays[counter]);
            
            
            std::string move = plays[counter];
            
            std::string result = net::getFromServer("http://www.bencarle.com/chess/move/" + gameId + "/" + teamNum + "/" + teamSecret + "/" + move + "/");
            
            tex = (double) (tend - tstart) / CLOCKS_PER_SEC;
            //5 seconds back every move
            twhite -= (tex - 5);
            
            ++counter;
            current_state = res;
            
            if (current_state.b_pieces[15] == CAPTURED || tblack <= 0)
            {
                white_won = true;
                break;
            }
        }
        // end white
        
        
        
        //black
        if(color == "black") {
            tstart = clock();
            tend = clock();
            
            std::string opponentMove = poll(gameId, teamNum + "/" + teamSecret);
            
            std::string pieceStr = opponentMove.substr(1, 2);
            std::string locationStr = opponentMove.substr(3, 2);
            int piece = getLocation(pieceStr);
            piece = getPiece(piece, "white", &current_state);
            int location = getLocation(locationStr);
            
            
            
            if(opponentMove == "Ke1g1" | opponentMove == "Ke1c1" | opponentMove == "Ke8g8" | opponentMove == "Ke8c8" | opponentMove.length() == 6) {
                moveSpecial(piece, location, true, &current_state, &res, (true) ? W_Q : B_Q);
                current_state = res;
            } else {
                makeMove(piece, location, true, &current_state, &res);
                current_state = res;
            }
            
            selectBestMove(false, &current_state, &res, b_ply, tblack);
            
            getMoveString(&res, &current_state, false, plays[counter]);
            
            std::string move = plays[counter];
            
            std::string result = net::getFromServer("http://www.bencarle.com/chess/move/" + gameId + "/" + teamNum + "/" + teamSecret + "/" + move + "/");
            
            
            
            tex = (double) (tend - tstart) / CLOCKS_PER_SEC;
            //5 seconds back every move
            tblack -= (tex - 5);
            
            ++counter;
            current_state = res;
            
            if (current_state.w_pieces[15] == CAPTURED || twhite <= 0)
            {
                white_won = false;
                break;
            }
            
            if (counter == 0)
            {
                puts("stalemate maybe...");
                draw = true;
                break;
            }
        }
        // end black
    }
    
    return 0;
};

std::string poll(std::string gameId, std::string teamNumSec) {
    Json::Value root;
    Json::Reader reader;
    bool ready = false;
    std::string res;
    
    while(!ready) {
        res = net::getFromServer("http://www.bencarle.com/chess/poll/" + gameId + "/" +  teamNumSec + "/");
        reader.parse(res, root);
        ready = root.get("ready", "default").asBool();
        std::cout << ready;
    }
    return root.get("lastmove", "default").asString();
};

int getLocation(std::string location) {
    std::string col = location.substr(0, 1);
    std::string row = location.substr(1, 1);
    std::stringstream ss;
    int rowId;
    int colId;
    ss << row;
    ss >> rowId;
    rowId--;
    if(col == "a") {
        colId = 0;
    } else if(col == "b") {
        colId = 1;
    } else if(col == "c") {
        colId = 2;
    } else if(col == "d") {
        colId = 3;
    } else if(col == "e") {
        colId = 4;
    } else if(col == "f") {
        colId = 5;
    } else if(col == "g") {
        colId = 6;
    } else if(col == "h") {
        colId = 7;
    }
    
    return (rowId * 8) + colId;
}


int getPiece(int location, std::string color, chessboard* board) {
    if(color == "white") {
        for(int i = 0; i < 16; ++i) {
            if(board->w_pieces[i] == location) {
                return i;
            }
        }
    } else if(color == "black") {
        for(int i = 0; i < 16; ++i) {
            if(board->b_pieces[i] == location) {
                return i;
            }
        }
    }
    return -1;
};