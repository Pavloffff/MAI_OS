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

void merge(intVector *v1, intVector *v2, int l, int r, int mid)
{
    int cnt = l, i = l, j = mid + 1;
    while (i <= mid && j <= r) {
        if (v1->begin[i] <= v2->begin[j]) {
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

void vSortTmp(intVector *v1, intVector *v2, int l, int r)
{
    if (l == r) {
        return;
    }
    int mid = (l + r) / 2;
    vSortTmp(v1, v2, l, mid);
    vSortTmp(v1, v2, mid + 1, r);
    merge(v1, v2, l, r, mid);
}

void vSort(intVector *v)
{
    intVector v2;
    vCreate(&v2);
    for (int i = 0; i < vSize(v); i++) {
        vPush(&v2, v->begin[i]);
    }
    vSortTmp(v, &v2, 0, vSize(v) - 1);
    vDestroy(&v2);
}