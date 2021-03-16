#ifndef __PRAK_STOLAR_ROBOT_TRADINGLOGLIST_HPP_
#define __PRAK_STOLAR_ROBOT_TRADINGLOGLIST_HPP_
enum TradingAction
{
    Bought,
    Sold
};
struct TradingResult
{
    int turnNumber;
    TradingAction action;
    char* playerName;
    int amount;
    int price;

    TradingResult() { playerName = new char[128]; }
    ~TradingResult()
    {
        printf("in trdingresult destructor: %s\n", playerName);
        delete playerName;
    }

    const TradingResult &operator=(const TradingResult& tr)
    {
        printf("in operator= of TradingResult\n");
        strcpy(playerName, tr.playerName);
        turnNumber = tr.turnNumber;
        action = tr.action;
        amount = tr.amount;
        price = tr.price;
        return *this;
    }
    
};
class TradingLogList
{
    struct Node
    {
        TradingResult data;
        Node* next;
    };
    Node* head;

public:
    TradingLogList();
    ~TradingLogList();
    void printList();
    void add(const TradingResult &log_line);
};
#endif // __PRAK_STOLAR_ROBOT_TRADINGLOGLIST_HPP_