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

sessionMap *sMapFind(sessionMap *iter, Session *key);
sessionMap *sMapInsert(sessionMap *iter, Session *key);
sessionMap *sMapRemove(sessionMap *iter, Session *key);
void sMapPrint(sessionMap *iter, int depthValue);
sessionMap *sMapDestroy(sessionMap *iter);
void sessionPrint(Session s);

#endif