#ifndef __SESSIONMAP_H__
#define __SESSIONMAP_H__

#include <string.h>
#include "kptools.h"
#include <stdbool.h>
#include "playervector.h"

typedef struct
{
    char *sessionName;
    int _sz;
    unsigned int cntOfPlayers;
    int currentPlayerIndex;
    playerVector *playersList;
    int hiddenNum;
} Session;

typedef struct sessionMap
{
    Session *key;
    int height;
    struct sessionMap *left;
    struct sessionMap *right;
} sessionMap;

Session *sMapFind(sessionMap *it, Session *key);
sessionMap *sMapInsert(sessionMap *it, Session *key);
sessionMap *sMapDelete(sessionMap *it, Session *key);
void sMapPrint(sessionMap *it, int depth);
sessionMap *sMapDestroy(sessionMap *it);
void sessionPrint(Session s);

#endif