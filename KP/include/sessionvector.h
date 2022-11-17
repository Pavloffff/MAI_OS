#ifndef __SESSIONVECTOR_H__
#define __SESSIONVECTOR_H__

#include <string.h>
#include "kptools.h"
#include <stdbool.h>

typedef struct
{
    char *sessionName;
    unsigned int cntOfPlayers;
    int currentPlayerIndex;
    playerVector *playersList;
    unsigned int hiddenNum;
} Session;

typedef struct
{
    Session *begin;
    int size;
    int capasity;
} sessionVector;

bool svIsEmpty(sessionVector *v);
bool svIsFull(sessionVector *v);
size_t svSize(sessionVector *v);
void svCreate(sessionVector *v);
void svDestroy(sessionVector *v);
void svPush(sessionVector *v, Session value);
Session svPop(sessionVector *v);
void sessionPrint(Session s);

#endif