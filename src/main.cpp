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


std::string getOppMove(std::string gameId);

int main(int argc, const char **argv) {
    using namespace std;
    
    std::string gameId = "GAMEID";
    bool playing = true;
    while(playing) {
        std::string opponentMove = getOppMove(gameId);
        std::string move = "Our Move";
        std::string teamNumSec = "Team_Number/Team_Secret";
        
        std::cout << move;

        std::string result = net::getFromServer("http://www.bencarle.com/chess/move/" + gameId + "/" + teamNumSec + "/" + move + "/");
    }
    return 0;
}


std::string getOppMove(std::string gameId) {
    Json::Value root;
    bool ready = false;
    std::string res;
    std::string teamNumSec = "Team_Number/Team_Secret";

    while(!ready) {
        res = net::getFromServer("http://www.bencarle.com/chess/poll/" + gameId + "/" +  teamNumSec + "/");
        ready = root.get(res, "ready").asBool();
        std::cout << res <<std::endl;
        std::cout << ready << std::endl;
    }
    return root.get(res, "prevMove").asString();
}
