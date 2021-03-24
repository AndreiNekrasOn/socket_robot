#ifndef __PRAK_STOLAR_ROBOT_PLAYERSSTATESLIST_HPP_
#define __PRAK_STOLAR_ROBOT_PLAYERSSTATESLIST_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct State
{
    char* name;
    int money;
    int raw;
    int plants;
    int prod;
    State();
    State(const State& st);

    ~State()
    {
        // printf("in state destructor: %s\n", name);
        delete name; // if comment it fixes all
        name = NULL; // so it's noop
    }
    const State& operator=(const State& st)
    {
        // printf("in operator= of state\n");
        strcpy(name, st.name);
        money = st.money;
        raw = st.raw;
        plants = st.plants;
        prod = st.prod;
        return *this;
    }
};

class PlayersStatesList
{
    struct Node
    {
        State data;
        Node* next;
    };
    Node* head;

public:
    PlayersStatesList();
    ~PlayersStatesList();
    Node* findByName(const char* name);
    void updateAndAdd(State st); // doesnt work if pass by value
    void printList();
};
#endif // __PRAK_STOLAR_ROBOT_PLAYERSSTATESLIST_HPP_