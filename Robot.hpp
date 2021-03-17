#ifndef __PRAK_STOLAR_ROBOT_ROBOT_HPP_
#define __PRAK_STOLAR_ROBOT_ROBOT_HPP_
#include "MartketInfo.hpp"
#include "PlayersStatesList.hpp"
#include "TradingLogList.hpp"

class Robot
{
    static const int max_playres_size = 10;
    PlayersStatesList* playresStates;
    MarketInfo currentMarketInfo;
    TradingLogList log;
    int gameSocket;
    int playingStatus;
    int turnNumber;
    char *roboName;
    char *ip;
    int port;
    int nWaitPlayers;

public:
    Robot();
    Robot(char *adress, int p, char *name, int nWait);

    void connectToGame();

    char* recvMsg();
    void sendMsg(const char* msg);
    void printAndRecvMsg();

    int enterGame();
    void createGame();

    void waitForStart();
    void waitForTurnEnd();
    bool isGameFinished();

    void setStrategy();
    void makeTurn();

    void updatePlayersStates();
    void updateMarketInfo();
    void updateTradingLog(const char* log_msg);

    void run();
};

#endif // __PRAK_STOLAR_ROBOT_ROBOT_HPP_
