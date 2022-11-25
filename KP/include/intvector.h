#ifndef __INTVECTOR_H__
#define __INTVECTOR_H__

#include <string.h>
#include "kptools.h"
#include <stdbool.h>

typedef struct
{
    int *begin;
    int size;
    int capasity;
} intVector;

bool vIsEmpty(intVector *v);
bool vIsFull(intVector *v);
size_t vSize(intVector *v);
void vCreate(intVector *v);
void vDestroy(intVector *v);
void vPush(intVector *v, int value);
int vPop(intVector *v);
void vPrint(intVector *v);
void vSort(intVector *v);

#endif