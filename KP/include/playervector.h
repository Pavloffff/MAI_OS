#ifndef __PLAYERVECTOR_H__
#define __PLAYERVECTOR_H__

#include <string.h>
#include "kptools.h"
#include <stdbool.h>
#include "intvector.h"

typedef struct 
{
    int num;
    int bulls;
    int cows;
    unsigned int answer;
} Player;

typedef struct
{
    Player *begin;
    int size;
    int capasity;
} playerVector;

bool pvIsEmpty(playerVector *v);
bool pvIsFull(playerVector *v);
size_t pvSize(playerVector *v);
void pvCreate(playerVector *v);
void pvDestroy(playerVector *v);
void pvPush(playerVector *v, Player value);
Player pvPop(playerVector *v);
void playerPrint(Player p);
void pvPrint(playerVector *v);

#endif