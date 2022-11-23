#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "kptools.h"
#include "intvector.h"
#include "sessionvector.h"
#include "playervector.h"

void createSession(Player *player, char *sessionName, int cntOfPlayers)
{
    int bulls = 0;
    int cows = 0;
    player->bulls = bulls;
    player->cows = cows;
    player->answer = 0;
    Session session;
    session.sessionName = sessionName;
    session._sz = strlen(sessionName) + 1;
    session.cntOfPlayers = (unsigned int) cntOfPlayers;
    srand(time(NULL));
    int answer = 1000 + rand() % (9999 - 1000 + 1);
    session.hiddenNum = answer;
    session.currentPlayerIndex = 0;
    playerVector players;
    pvCreate(&players);
    pvPush(&players, *player);
    session.playersList = &players;
    sessionPrint(session);
    int mainFd = shm_open("main.back", O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    sem_t *mainSem = sem_open("main.semaphore", O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH, 0);
    int mainSz = sizeof(Session);
    mainSz += session._sz;
    mainSz += sizeof(playerVector);
    mainSz += session.playersList->capasity * (sizeof(Player)); 
    // printf("%d\n", mainSz);
    ftruncate(mainFd, mainSz);
    char *mapped = (char *) mmap(NULL,
                                 mainSz,
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED,
                                 mainFd,
                                 0);
    memcpy(mapped, &session, sizeof(Session));
    memcpy(mapped + sizeof(Session), session.sessionName, session._sz);
    memcpy(mapped + sizeof(Session) + session._sz, session.playersList, sizeof(playerVector));
    memcpy(mapped + sizeof(Session) + session._sz + sizeof(playerVector), 
           session.playersList->begin, session.playersList->capasity * (sizeof(Player)));
    // for (int i = 0; i <= mainSz; i++) {
    //     printf("%d ", mapped[i]);
    // }
    // printf("\n");
    int state = 0;
    sem_getvalue(mainSem, &state);
    while (state++ < 1) {
        sem_post(mainSem);
    }
    while (state-- > 2) {
        sem_wait(mainSem);
    }
    printf("Session created\n");
    munmap(mapped, mainSz);
    close(mainFd);
    sem_close(mainSem);
}

int main(int argc, char const *argv[])
{
    Player *player;
    player->num = 1;
    char *command;
    int flag = 1;
    while (flag) {
        printf("Input \"Create\" to create new session\n");
        printf("Input \"Join\" to join exist session by session name\n");
        printf("Input \"Find\" to find random session\n");
        command = getString();
        if (strcmp(command, "Create") == 0) {
            printf("Input name of session which you want to create\n");
            char *sessionName = getString();
            printf("Input count of players\n");
            int cntOfPlayers = 0;
            scanf("%d", &cntOfPlayers);
            createSession(player, sessionName, cntOfPlayers);
            free(command);
            flag = 0;
        } else if (strcmp(command, "Join") == 0) {
            printf("session joined\n");
            free(command);
            flag = 0;
        } else if (strcmp(command, "Find") == 0) {
            printf("session finded\n");
            free(command);
            flag = 0;
        } else if (!feof(stdin)){
            printf("Wrong command! Try again\n");
            free(command);
            continue;
        } else {
            free(command);
            flag = 0;
        }
    }
    
    // intVector v; 
    // vCreate(&v);
    // vPush(&v, 5);
    // vPush(&v, 2048);
    // vPush(&v, -11);
    // vPrint(&v);
    // vDestroy(&v);
    return 0;
}
