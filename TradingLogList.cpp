#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TradingLogList.hpp"
TradingLogList::TradingLogList()
{
    head = NULL;
}
TradingLogList::~TradingLogList()
{
    while (head != NULL)
    {
        Node* n = head->next;
        delete head;
        head = n;
    }
}

void TradingLogList::add(const TradingResult& log_line)
{
    Node* n = new Node;
    n->data = log_line;
    n->next = head;
    head = n;
}

void TradingLogList::printList()
{
    // printf("\\printing trades log...\n");
    for (Node* i = head; i != NULL; i = i->next)
    {
        printf("Trading log: Turn#%d, Name: %s, Action: %s,  Amount: %d, "
               "Price: %d\n",
               i->data.turnNumber, i->data.playerName,
               i->data.action == Bought ? "Bought" : "Sold",
               i->data.amount, i->data.price);
    }
}