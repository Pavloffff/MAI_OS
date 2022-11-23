#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sessionvector.h"
#include "playervector.h"
#include "kptools.h"

bool svIsEmpty(sessionVector *v)
{
	return v->size == 0;
}

bool svIsFull(sessionVector *v)
{
	return v->size >= v->capasity;
}

size_t svSize(sessionVector *v)
{
	return v->size;
}

void svCreate(sessionVector *v)
{
	v->size = 0;
	v->capasity = 1;
	v->begin = malloc(sizeof(Session));
}

void svDestroy(sessionVector *v)
{
	free(v->begin);
	v->size = 0;
	v->capasity = 0;
	v->begin = NULL;
}

void svPush(sessionVector *v, Session value)
{
	if (svIsFull(v)) {
		v->capasity *= 2;
		v->begin = realloc(v->begin, v->capasity * sizeof(Session));
	}
	v->begin[v->size] = value;
	v->size++;
}

Session svPop(sessionVector *v)
{
	Session res = v->begin[v->size - 1];
	v->size--;
	return res;
}

void sessionPrint(Session s)
{
    printf("Name of session: %s\n", s.sessionName);
    printf("sz: %d\n", s._sz);
    printf("Count of players: %d\n", s.cntOfPlayers);
    printf("Turn of player: %d\n", s.currentPlayerIndex);
    printf("Players:\n");
    pvPrint(s.playersList);
    printf("hidden Number: %d\n", s.hiddenNum);
}