#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "labtools.h"

int main(int argc, char const *argv[])
{
    std::string in = myInput();
    int fd = shm_open(backFile.c_str(), O_RDWR | O_CREAT, accessPerm);
    CHECK_ERROR(fd, -1, "shm_open"); 
    sem_t *semaphore = sem_open(semFile.c_str(), O_CREAT, accessPerm);
    CHECK_ERROR(semaphore, SEM_FAILED, "sem_open");
    int state = 0, mapSize = in.size() + 1;
    ftruncate(fd, (int) mapSize);
    char * mapped = (char *) mmap(NULL,
                                    mapSize,
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED,
                                    fd,
                                    0);
    CHECK_ERROR(mapped, MAP_FAILED, "mmap");
    memset(mapped, '\0', mapSize);
    sprintf(mapped, "%s", in.c_str());
    int semErrCheck = sem_getvalue(semaphore, &state);
    CHECK_ERROR(semErrCheck, -1, "sem_getvalue");
    while (state++ < 2) {
        sem_post(semaphore);
    }
    while (state-- > 3) {
        sem_post(semaphore);
    }
    pid_t child1Id = fork();
    CHECK_ERROR(child1Id, -1, "fork");
    if (child1Id == 0) {
        munmap(mapped, mapSize);
        close(fd);
        sem_close(semaphore);
        int execlErrCheck = execl("child1", "child1", NULL);
        CHECK_ERROR(execlErrCheck, 0, "execl");
    }
    while (1) {
        int semErrCheck = sem_getvalue(semaphore, &state);
        CHECK_ERROR(semErrCheck, -1, "sem_getvalue");
        if (state == 0) {
            int semWaitErrCheck = sem_wait(semaphore);
            CHECK_ERROR(semWaitErrCheck, -1, "sem_wait");
            std::cout << mapped;
            return 0;
        }
    }
    return 0;
}