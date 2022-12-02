#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sessionmap.h"
#include "playervector.h"
#include "kptools.h"

int sessionCmp(Session *s1, Session *s2)
{
    return strcmp(s1->sessionName, s2->sessionName);
}

int max(int x, int y)
{
    return x > y ? x : y;
}

int getHeight(sessionMap *it)
{
    if (it == NULL) {
        return 0;
    }
    return it->height;
}

void setHeight(sessionMap *it)
{
    int hl = getHeight(it->left);
    int hr = getHeight(it->right);
    if (hl > hr) {
        it->height = hl + 1;
    } else {
        it->height = hr + 1;
    }
}


int balanceFactor(sessionMap *it)
{
    return getHeight(it->right) - getHeight(it->left);
}

sessionMap *rotateLeft(sessionMap *it)
{
    sessionMap *it1 = it->right;
    it->right = it1->left;
    it1->left = it;
    setHeight(it1);
    setHeight(it);
    return it1;
}

sessionMap *rotateRight(sessionMap *it)
{
    sessionMap *it1 = it->left;
    it->left = it1->right;
    it1->right = it;
    setHeight(it1);
    setHeight(it);
    return it1;
}

sessionMap *balance(sessionMap *it)
{
    setHeight(it);
    if (balanceFactor(it) == 2) {
        if (balanceFactor(it->right) < 0) {
            it->right = rotateRight(it->right);
        }
        it = rotateLeft(it);
    } else if (balanceFactor(it) == -2) {
        if (balanceFactor(it->left) > 0) {
            it->left = rotateLeft(it->left);
        }
        it = rotateRight(it);
    }
    return it;
}

sessionMap *sMapInsert(sessionMap *it, Session *key)
{
    if (it == NULL) {
        sessionMap *tmp = malloc(sizeof(sessionMap));
        tmp->key = key;
        tmp->height = 1;
        tmp->left = NULL;
        tmp->right = NULL;
        return tmp;
    } else {
        if (sessionCmp(it->key, key) == 0) {
        } else if (sessionCmp(it->key, key) > 0) {
            it->left = sMapInsert(it->left, key);
        } else {
            it->right = sMapInsert(it->right, key);
        }
        it = balance(it);
    }
    return it;
}

sessionMap *minimalsessionMap(sessionMap *it)
{
    while (it->left != NULL) {
        it = it->left;
    }
    return it;
}

sessionMap *sMapDeleteMinimalsessionMap(sessionMap *it)
{
    if (it->left == NULL) {
        return it->right;
    }
    it->left = sMapDeleteMinimalsessionMap(it->left);
    return balance(it);
}

sessionMap *sMapDelete(sessionMap *it, Session *key)
{
    if (it == NULL) {
    } else {
        if (sessionCmp(it->key, key) > 0) {
            it->left = sMapDelete(it->left, key);
        } else if (sessionCmp(it->key, key) < 0) {
            it->right = sMapDelete(it->right, key);
        } else {
            sessionMap *l = it->left;
            sessionMap *r = it->right;
            free(it);
            it = NULL;
            if (r == NULL) {
                return r;
            }
            sessionMap *min = minimalsessionMap(r);
            min->right = sMapDeleteMinimalsessionMap(r);
            min->left = l;
            return balance(min);
        }
    }
    return balance(it);
}

int Find(sessionMap *it, Session *key)
{
    if (it == NULL) {
        return 0;
    } else {
        if (sessionCmp(it->key, key) > 0) {
            if (!Find(it->left, key)) {
                return 0;
            }
        } else if (sessionCmp(it->key, key) < 0) {
            if (!Find(it->right, key)) {
                return 0;
            }
        } else {
            return 1;
        }
    }
    return 1;
}

sessionMap *sMapDestroy(sessionMap *it)
{
	if (it == NULL)
		return it;
	if (it->left == NULL && it->right == NULL) {
		free(it);
		return NULL;
	}
	if (it->left != NULL)
		it->left = sMapDestroy(it->left);
	if (it->right != NULL)
		it->right = sMapDestroy(it->right);
	return sMapDestroy(it);
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

void sMapPrint(sessionMap *it, int depth)
{
    if (it != NULL) {
        for (int i = 0; i < depth; ++i) {
            printf("\t");
        }
        printf("%s", it->key->sessionName); printf("\n");
        sMapPrint(it->left,  depth + 1);
        sMapPrint(it->right, depth + 1);
    }
}