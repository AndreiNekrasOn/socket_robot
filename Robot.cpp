#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "Robot.hpp"

Robot::Robot()
{
    roboName = new char[128];
    strcpy(roboName, "Robot");
    port = 4774;
    ip = new char[128];
    strcpy(ip, "127.0.0.1");
    nWaitPlayers = 2;
    playingStatus = 0;
    turnNumber = 0;
    playresStates = new PlayersStatesList;
}

Robot::Robot(char *adress, int p,  char *name, int nWait)
{
    ip = new char[128];
    roboName = new char[128];
    strcpy(ip, adress);
    strcpy(roboName, name);
    roboName = name;
    nWaitPlayers = nWait;
    port = p;
    playresStates = new PlayersStatesList;
    turnNumber = 0;
    playingStatus = 0;
}

void Robot::connectToGame()
{
    int game_socket = 0;
    sockaddr_in serv_addr;
    if ((game_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Robot::connectToGame error, socket returned < 0\n");
        exit(1);
    }
    printf("addr=%s port=%d\n", this->ip, this->port);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(this->port);
    serv_addr.sin_addr.s_addr = inet_addr(this->ip);

    if (connect(game_socket, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Robot::connectToGame error, connect failed\n");
        exit(1);
    }
    printf("Successfully connected\n");
    this->gameSocket = game_socket;
}

void Robot::sendMsg(const char* msg)
{
    printf("> %s", msg);
    write(this->gameSocket, msg, strlen(msg));
}

char* Robot::recvMsg()
{
    char* buffer = new char[1024];
    int rc = read(this->gameSocket, buffer, 1023);
    buffer[rc] = 0;
    printf("%s\n", buffer);
    return buffer;
}

void Robot::printAndRecvMsg()
{
    char* buffer = recvMsg();
    delete buffer;
}

int Robot::enterGame()
{
    printAndRecvMsg(); // greeting msg to trash, where it belongs!
    int suckass = 0;
    const char* game_indicator = "waiting  #";
    sendMsg(".who\n");
    char* answer = recvMsg();
    char* waiting_pos = strstr(answer, game_indicator);
    int game_number;
    if (waiting_pos != NULL)
    {
        sscanf(waiting_pos + strlen(game_indicator), "%d", &game_number);
        char buf[256];
        sprintf(buf, ".join %d\n", game_number);
        sendMsg(buf);
        printAndRecvMsg();
        suckass = 1;
    }
    delete answer;
    return suckass;
}

void Robot::createGame()
{
    sendMsg(".create\n");
    printAndRecvMsg();
    const char *join_msg = "@+ JOIN";
    int players_counter = 1;
    printf("waiting for %d players\n", nWaitPlayers);
    while(1)
    {
        char *msg = recvMsg();
        if (strstr(msg, join_msg) != NULL)
        {
            players_counter++;
        }
        if (players_counter == nWaitPlayers)
        {
            break;
        }
        delete msg;
    }
    sendMsg("start\n");
}

void Robot::waitForStart()
{
    char* msg;
    const char* start_msg = "& START";
    bool flag = true;
    while (flag)
    {
        msg = recvMsg();
        flag = strncmp(msg, start_msg, strlen(start_msg)) != 0;
        printf("\\flag is %d\n", flag);
        delete msg;
    }
}

void Robot::updateMarketInfo()
{
    char* msg;
    const char* market = "& MARKET";
    sendMsg("market\n");
    msg = recvMsg();
    while(strstr(msg, market) == NULL)
    {
        delete msg;
        sendMsg("market\n");
        msg = recvMsg();
    }
    sscanf(strstr(msg, market) + strlen(market), "%d%d%d%d",
        &(currentMarketInfo.raw), &(currentMarketInfo.minPrice),
        &(currentMarketInfo.prod), &(currentMarketInfo.maxPrice));
    delete msg;
}

void Robot::updateTradingLog(const char* log_msg)
{
    printf("%s\n", log_msg);
    const char* end_prefix = "& ENDTURN";
    const char* bankrupt_prefix = "& BANKRUPT";
    const char* bought_prefix = "& BOUGHT"; // max prefix chosen
    const char* table_entry_label = "& ";
    char* msg = new char[strlen(log_msg) + 1];
    strcpy(msg, log_msg);
    TradingResult res;
    char* tmp = msg;
    tmp = strstr(tmp, table_entry_label);
    if (!strstr(tmp, "Trading results")) {
        fprintf(stderr, "Incorrect massege to parse ?");
        exit(1);
    }
    while (strncmp(tmp, end_prefix, strlen(end_prefix)) != 0 && strncmp(tmp, bankrupt_prefix, strlen(bankrupt_prefix)) != 0)
    {
        res.turnNumber = this->turnNumber;
        res.action
            = strncmp(tmp, bought_prefix, strlen(bought_prefix)) == 0
            ? Bought
            : Sold;
        tmp += strlen(bought_prefix);
        sscanf(tmp, "%s%d%d", res.playerName, &res.amount, &res.price);
        printf("\\log parsed: %s %s %d %d$\n",
               res.action == Bought ? "Bought" : "Sold", res.playerName,
               res.amount, res.price);
        this->log.add(res);
        tmp = strstr(tmp, table_entry_label);
    }
}

void Robot::makeTurn()
{
    char* action_msg = new char[256];
    updatePlayersStates();
    this->playresStates->printList();
    updateMarketInfo();

    sprintf(action_msg, "buy 2 %d\n", currentMarketInfo.minPrice);
    sendMsg(action_msg);
    printAndRecvMsg();

    sprintf(action_msg, "sell %d %d\n",
            playresStates->findByName(roboName)->data.prod,
            currentMarketInfo.maxPrice);
    sendMsg(action_msg);
    printAndRecvMsg();

    sprintf(action_msg, "prod %d\n",
            playresStates->findByName(roboName)->data.raw);
    sendMsg(action_msg);
    printAndRecvMsg();

    sendMsg("turn\n");

    delete action_msg;
}

void Robot::waitForTurnEnd()
{
    bool flag = true;
    const char* end_turn_msg = "& ENDTURN";
    char* msg;
    while (flag)
    {
        msg = recvMsg();
        flag = strstr(msg, end_turn_msg) != NULL;
        delete msg;
    }
    turnNumber++;
}

bool Robot::isGameFinished()
{
    const char* winner = "WINNER";
    const char* bankrupt = "BANKRUPT";
    const char* game_over = "&- The game is over, type quit to quit...";
    char* msg = recvMsg();
    bool flag = strstr(msg, winner) != NULL
        || strstr(msg, bankrupt) != NULL || strstr(msg, game_over);

    updateTradingLog(msg);
    if (flag)
    {
        printf("\\last msg:\n%s\n", msg);
    }
    delete msg;
    return flag;
}

// int k = 0;
void Robot::updatePlayersStates()
{
    playresStates = new PlayersStatesList(); 

    sendMsg("info\n");
    char* info_msg = recvMsg();
    const char* table_entry_label = "& INFO";
    char* tmp = info_msg;
    tmp = strstr(tmp, table_entry_label);

    while (tmp != NULL)
    {
        State st;
        tmp += strlen(table_entry_label);
        sscanf(tmp, "%s%d%d%d%d", st.name, &st.raw, &st.prod, &st.money,
               &st.plants);
        // if (strcmp(st.name, "Robot") == 0) { st.money -= k * 1000; k++; } // DEBUG
        printf("\\parsed info: player %s\n", st.name);
        this->playresStates->updateAndAdd(st);
        this->playresStates->printList();
        printf("\\update suckassfull\n");

        tmp = strstr(tmp, table_entry_label);
    }
    printf("TOTAL LIST AFTER ITERATION\n");
    this->playresStates->printList();
    printf("Player Robot in list %d\n",
           this->playresStates->findByName(roboName) != NULL);
    delete info_msg;
}

// void Robot::updatePlayersStates()
// {
//     if (1 == 1)
//     {
//         State s1, s2, s3;
//         sscanf("dummyname1", "%s", s1.name);
//         sscanf("dummyname2", "%s", s2.name);
//         s2.money = 999;
//         sscanf("dummyname3", "%s", s3.name);
//         printf("%s %s %s\n", s1.name, s2.name, s3.name);
//         this->playresStates->updateAndAdd(s1);
//         this->playresStates->updateAndAdd(s2);
//         this->playresStates->updateAndAdd(s3);
//     }

//     this->playresStates->printList();
// }

void Robot::run()
{
    printAndRecvMsg();
    char* action_msg = new char[256];
    sprintf(action_msg, "%s\n", roboName);
    sendMsg(action_msg);
    if (!enterGame())
    {
        createGame();
    }
    waitForStart();
    bool flag = true;
    while (flag)
    {
        makeTurn();
        waitForTurnEnd();
        flag = !isGameFinished();
    }
    this->log.printList();
}
