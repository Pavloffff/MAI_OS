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

int height(sessionMap *iter)
{
    if (iter == NULL) {
        return 0;
    }
    return iter->height;
}

int max(int x, int y)
{
    return (x > y) ? x : y;
}

sessionMap *sMapFind(sessionMap *iter, Session *key)
{
    if (iter == NULL) {
        return NULL;
    } else if (sessionCmp(key, iter->key) == -1) {
        return sMapFind(iter->left, key);
    } else if (sessionCmp(key, iter->key) == 1) {
        return sMapFind(iter->right, key);
    } else {
        return iter;
    }
}

sessionMap *rightRotate(sessionMap *iter)
{
    sessionMap *it1 = iter->left;
    sessionMap *it2 = iter->right;
    it1->right = iter;
    iter->left = it2;
    iter->height = max(height(iter->left), height(iter->right)) + 1;
    it1->height = max(height(it1->left), height(it1->right)) + 1;
    return it1;
}

sessionMap *leftRotate(sessionMap *iter)
{
    sessionMap *it1 = iter->right;
    sessionMap *it2 = it1->left;
    it1->left = iter;
    iter->right = it2;
    iter->height = max(height(iter->left), height(iter->right)) + 1;
    it1->height = max(height(it1->left), height(it1->right)) + 1;
    return it1;
}

int getBalance(sessionMap *iter)
{
    if (iter == NULL) {
        return 0;
    }
    return height(iter->left) - height(iter->right);
}

sessionMap *sMapInsert(sessionMap *iter, Session *key)
{
    if (iter == NULL) {
        sessionMap *tmp = malloc(sizeof(sessionMap));
        tmp->key = key;
        tmp->height = 1;
        tmp->left = NULL;
        tmp->right = NULL;
        return tmp;
    } else if (sessionCmp(key, iter->key) == -1) {
        iter->left = sMapInsert(iter->left, key);
    } else if (sessionCmp(key, iter->key) == 1) {
        iter->right = sMapInsert(iter->right, key);
    } else {
        iter->key = key;
        return iter;
    }
    iter->height = 1 + max(height(iter->left), height(iter->right));
    int balance = getBalance(iter);
    if (balance > 1 && sessionCmp(key, iter->left->key) == -1) {
        return rightRotate(iter);
    }
    if (balance < -1 && sessionCmp(key, iter->right->key) == 1) {
        return leftRotate(iter);
    }
    if (balance > 1 && sessionCmp(key, iter->left->key) == 1) {
        iter->left = leftRotate(iter->left);
        return rightRotate(iter);
    }
    if (balance < 1 && sessionCmp(key, iter->left->key) == -1) {
        iter->right = rightRotate(iter->right);
        return leftRotate(iter);
    }
    iter->key = key;
    return iter;
}

sessionMap *minElem(sessionMap *iter)
{
    if (iter->left == NULL) {
        return iter;
    }
    return minElem(iter->left);
}

// удаляются ТОЛЬКО ссылки, можно потом дописать и удаление памяти Session, а можно это сделать не тут
sessionMap *sMapRemove(sessionMap *iter, Session *key)
{
    if (iter == NULL) {
        return iter;
    }
    if (sessionCmp(key, iter->key) == -1) {
        iter->left = sMapRemove(iter->left, key);
    } else if (sessionCmp(key, iter->key) == 1) {
        iter->right = sMapRemove(iter->right, key);
    } else if (iter->left != NULL && iter->right != NULL) {
        iter->key = minElem(iter->right)->key;
        iter->right = sMapRemove(iter->right, iter->key);
    } else if (iter->left != NULL) {
        sessionMap *leftUndertree = iter->left;
        free(iter);
        iter = leftUndertree;
    } else if (iter->right != NULL) {
        sessionMap *rightUndertree = iter->right;
        free(iter);
        iter = rightUndertree;
    } else {
        free(iter);
        iter = NULL;
    }
    return iter;
}

void sMapPrint(sessionMap *iter, int depthValue)
{
    if (iter != NULL) {
        for (int i = 0; i < depthValue; ++i) {
			printf("\t\t");
		}
        sessionPrint(*iter->key);
        printf("\t\t\n");
        sMapPrint(iter->left, depthValue + 1);
        sMapPrint(iter->right, depthValue + 1);
    }
}

sessionMap *sMapDestroy(sessionMap *iter)
{
    if (iter == NULL) {
        return iter;
    }
    if (iter->left == NULL && iter->right == NULL) {
        free(iter);
        return NULL;
    }
    if (iter->left != NULL) {
        iter->left = sMapDestroy(iter->left);
    }
    if (iter->right != NULL) {
        iter->right = sMapDestroy(iter->right);
    }
    return sMapDestroy(iter);
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