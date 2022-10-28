#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "labtools.h"

int main(int argc, char const *argv[])
{
    int fd = shm_open(backFile.c_str(), O_RDWR, accessPerm);
    CHECK_ERROR(fd, -1, "shm_open");
    struct stat statBuf;
    fstat(fd, &statBuf);
    const size_t mapSize = statBuf.st_size;
    char * mapped = (char *) mmap(NULL,
                                    mapSize,
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED,
                                    fd,
                                    0);
    CHECK_ERROR(mapped, MAP_FAILED, "mmap");
    sem_t *semaphore = sem_open(semFile.c_str(), O_CREAT, accessPerm, 2);
    CHECK_ERROR(semaphore, SEM_FAILED, "sem_open");
    int semWaitErrCheck = sem_wait(semaphore);
    CHECK_ERROR(semWaitErrCheck, -1, "sem_wait");
    std::string str;
    str = mapped;
    std::transform(str.begin(),
                   str.end(),
                   str.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    memset(mapped, '\0', mapSize);
    sprintf(mapped, "%s", str.c_str());
    pid_t child2Id = fork();
    CHECK_ERROR(child2Id, -1, "fork");
    if (child2Id == 0) {
        munmap(mapped, mapSize);
        close(fd);
        sem_close(semaphore);
        int execlErrCheck = execl("child2", "child2", NULL);
        CHECK_ERROR(execlErrCheck, 0, "execl");
    }
    return 0;
}