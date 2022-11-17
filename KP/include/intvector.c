#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "intvector.h"

bool vIsEmpty(intVector *v)
{
	return v->size == 0;
}

bool vIsFull(intVector *v)
{
	return v->size >= v->capasity;
}

size_t vSize(intVector *v)
{
	return v->size;
}

void vCreate(intVector *v)
{
	v->size = 0;
	v->capasity = 1;
	v->begin = malloc(sizeof(int));
}

void vDestroy(intVector *v)
{
	free(v->begin);
	v->size = 0;
	v->capasity = 0;
	v->begin = NULL;
}

void vPush(intVector *v, int value)
{
	if (vIsFull(v)) {
		v->capasity *= 2;
		v->begin = realloc(v->begin, v->capasity * sizeof(int));
	}
	v->begin[v->size] = value;
	v->size++;
}

int vPop(intVector *v)
{
	int res = v->begin[v->size - 1];
	v->size--;
	return res;
}

void vPrint(intVector *v)
{
    for (int i = 0; i < vSize(v); i++) {
        printf("%d ", v->begin[i]);
    }
    printf("\n");
}
