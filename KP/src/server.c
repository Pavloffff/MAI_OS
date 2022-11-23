#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "kptools.h"
#include "intvector.h"
#include "sessionvector.h"
#include "playervector.h"

int main(int argc, char const *argv[])
{
    // printf("hello world\n");
    sem_t *mainSem = sem_open("main.semaphore", O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH, 0);
    int state;
    sem_getvalue(mainSem, &state);
    while (state++ < 0) {
        sem_post(mainSem);
    }
    while (state-- > 1) {
        sem_wait(mainSem);
    }
    while (1) {
        sem_getvalue(mainSem, &state);
        if (state == 1) {
            int mainFd = shm_open("main.back", O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
            struct stat statBuf;
            fstat(mainFd, &statBuf);
            int mapSize = statBuf.st_size;
            printf("%d\n", mapSize);
            Session *session = malloc(sizeof(Session));
            char *mapped = (char *)mmap(NULL,
                                mapSize,
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED,
                                mainFd,
                                0);
            memcpy(session, mapped, sizeof(Session));
            session->sessionName = malloc(session->_sz);
            memcpy((void *) session->sessionName, mapped + sizeof(Session), session->_sz);
            session->playersList = malloc(sizeof(playerVector));
            memcpy(session->playersList, mapped + sizeof(Session) + session->_sz, sizeof(playerVector));
            session->playersList->begin = malloc(session->playersList->capasity * sizeof(Player));
            memcpy(session->playersList->begin, mapped + sizeof(Session) + session->_sz + sizeof(playerVector), 
                   session->playersList->capasity * sizeof(Player));
            sessionPrint(*session);
            munmap(mapped, mapSize);
            close(mainFd);
            sem_post(mainSem);
            continue;
        }
    }
    sem_close(mainSem);
    return 0;
}
