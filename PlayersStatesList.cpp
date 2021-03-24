#include <stdio.h>
#include <string.h>

#include "PlayersStatesList.hpp"

State::State()
    : money(10000)
    , raw(4)
    , plants(2)
    , prod(2)
{
    // printf("in the constructor of state\n");
    name = new char[128];
}

State::State(const State& st)
    : money(st.money)
    , raw(st.raw)
    , plants(st.plants)
    , prod(st.prod)
{
    // printf("in copy constructor of state for ''%s''\n", st.name);
    name = new char[128];
    // printf("able to new\n");
    strcpy(name, st.name);
    // printf("able to strcpy\n");
}

PlayersStatesList::PlayersStatesList()
{
    head = NULL;
}

PlayersStatesList::~PlayersStatesList()
{
    // printf("PANIC PANIC PANIC IN DESTRUCTOR OF PLAYERS STATES LIST\n");
    while (head != NULL)
    {
        Node* n = head->next;
        delete head;
        head = NULL;
        head = n;
    }
}
PlayersStatesList::Node* PlayersStatesList::findByName(const char* name)
{
    // printf("in findByName\n");
    // printf("head is null? %d\n", head == NULL);
    printList();
    for (Node* i = head; i != NULL; i = i->next)
    {
        // printf("in for findByName\n");
        // printf("i->data is null? %d\n", i->data.name == NULL);
        if (strcmp(i->data.name, name) == 0)
        {
            return i;
        }
    }
    // printf("return null from findByName\n");
    return NULL;
}
void PlayersStatesList::updateAndAdd(State st)
{
    // printf("in updateAndAdd\n");
    // printf("st.name is null? %d\n", st.name == NULL);
    Node* player = findByName(st.name);
    if (player != NULL)
    {
        // printf("FOUND EXISTING PLAYR\n");
        player->data = st;
    }
    else
    {
        // printf("in updateAndAdd else branch\n");
        Node* n = new Node;
        n->data = st;
        // printf("node name %s\n", n->data.name);
        n->next = head;
        head = n;
    }
}

void PlayersStatesList::printList()
{
    if (head == NULL)
        return;
    // printf("printState head %s\n", head->data.name);
    for (Node* i = head; i != NULL; i = i->next)
    {
        State* st = &i->data;
        printf("\tname %s money %d raw %d prod %d plants %d\n", st->name,
               st->money, st->raw, st->prod, st->plants);
    }
}
