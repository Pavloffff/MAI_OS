#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "kptools.h"
#include "playervector.h"
#include "intvector.h"

bool pvIsEmpty(playerVector *v)
{
	return v->size == 0;
}

bool pvIsFull(playerVector *v)
{
	return v->size >= v->capasity;
}

size_t pvSize(playerVector *v)
{
	return v->size;
}

void pvCreate(playerVector *v)
{
	v->size = 0;
	v->capasity = 1;
	v->begin = malloc(sizeof(Player));
}

void pvDestroy(playerVector *v)
{
	free(v->begin);
	v->size = 0;
	v->capasity = 0;
	v->begin = NULL;
}

void pvPush(playerVector *v, Player value)
{
	if (vIsFull(v)) {
		v->capasity *= 2;
		v->begin = realloc(v->begin, v->capasity * sizeof(Player));
	}
	v->begin[v->size] = value;
	v->size++;
}

Player pvPop(playerVector *v)
{
	Player res = v->begin[v->size - 1];
	v->size--;
	return res;
}

void playerPrint(Player p)
{
    printf("Name: %d\n", p.name);
    printf("Bulls:\n");
    vPrint(p.bulls);
    printf("Cows:\n");
    vPrint(p.cows);
    printf("Last answer: %d\n", p.answer);
}

void pvPrint(playerVector *v)
{
    for (int i = 0; i < pvSize(v); i++) {
        playerPrint(v->begin[i]);
    }
}
