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
	if (pvIsFull(v)) {
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
    printf("num: %d\n", p.num);
    printf("Bulls: %d\n", p.bulls);
    printf("Cows: %d\n", p.cows);
    printf("Last answer: %d\n", p.answer);
}

void pvPrint(playerVector *v)
{
    for (int i = 0; i < pvSize(v); i++) {
        playerPrint(v->begin[i]);
    }
}

int comparator(Player p1, Player p2)
{
    if (p1.bulls > p2.bulls) {
        return 1;
    } else if ((p1.bulls == p2.bulls) && (p1.cows > p2.cows)) {
        return 1;
    } else {
        return 0;
    }
}

void pmerge(playerVector *v1, playerVector *v2, int l, int r, int mid)
{
    int cnt = l, i = l, j = mid + 1;
    while (i <= mid && j <= r) {
        if (comparator(v1->begin[i], v2->begin[j]) == 0) {
            v2->begin[cnt++] = v1->begin[i++];
        } else {
            v2->begin[cnt++] = v1->begin[j++];
        }
    }
    while (i <= mid) {
        v2->begin[cnt++] = v1->begin[i++];
    }
    for (int i = 0; i <= r; i++) {
        v1->begin[i] = v2->begin[i];
    }
}

void pvSortTmp(playerVector *v1, playerVector *v2, int l, int r)
{
    if (l == r) {
        return;
    }
    int mid = (r + l) / 2;
    pvSortTmp(v1, v2, l, mid);
    pvSortTmp(v1, v2, mid + 1, r);
    pmerge(v1, v2, l, r, mid);
}

void pvSort(playerVector *v)
{
    playerVector v2;
    pvCreate(&v2);
    for (int i = 0; i < pvSize(v); i++) {
        pvPush(&v2, v->begin[i]);
    }
    pvSortTmp(v, &v2, 0, pvSize(v) - 1);
    pvDestroy(&v2);
}
