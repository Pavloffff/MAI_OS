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
#include "../include/labtools.h"

int main(int argc, char const *argv[])
{
    sem_unlink(semFile.c_str());
    sem_t *semaphore = sem_open(semFile.c_str(), O_CREAT, accessPerm, 0);
    CHECK_ERROR(semaphore, SEM_FAILED, "sem_open");
    int state = 0;
    while (++state < 4) {
        sem_post(semaphore);
    }
    while (--state > 3) {
        sem_post(semaphore);
    }
    pid_t child1Id = fork();
    CHECK_ERROR(child1Id, -1, "fork");
    if (child1Id == 0) {
        sem_close(semaphore);
        int execlErrCheck = execlp("./child1", "./child1", NULL);
        CHECK_ERROR(execlErrCheck, 0, "execl");
    } else {
        pid_t child2Id = fork();
        CHECK_ERROR(child2Id, -1, "fork");
        if (child2Id == 0) {
            sem_close(semaphore);
            int execlErrCheck = execlp("./child2", "./child2", NULL);
            CHECK_ERROR(execlErrCheck, 0, "execl");
        } else {
            std::string str;
            int sz;
            int flag = 1;
            while (flag) {
                int semErrCheck = sem_getvalue(semaphore, &state);
                CHECK_ERROR(semErrCheck, -1, "sem_getvalue");
                if (state == 3) {
                    getline(std::cin, str);
                    sz = str.length() + 1;
                    int fd = shm_open(backFile.c_str(), O_RDWR | O_CREAT, accessPerm);
                    CHECK_ERROR(fd, -1, "shm_open");
                    ftruncate(fd, sz);
                    char *mapped = (char *) mmap(NULL, 
                                                sz,
                                                PROT_READ | PROT_WRITE,
                                                MAP_SHARED,
                                                fd,
                                                0);
                    CHECK_ERROR(mapped, MAP_FAILED, "mmap");
                    memset(mapped, '\0', sz);
                    sprintf(mapped, "%s", str.c_str());
                    semErrCheck = sem_getvalue(semaphore, &state);
                    CHECK_ERROR(semErrCheck, -1, "sem_getvalue");
                    close(fd);
                    while (++state < 3) {
                        sem_post(semaphore);
                    }
                    while (--state > 2) {
                        sem_wait(semaphore);
                    }
                }
                if (state == 0) {
                    int fd = shm_open(backFile.c_str(), O_RDWR | O_CREAT, accessPerm);
                    CHECK_ERROR(fd, -1, "shm_open");
                    struct stat statBuf;
                    fstat(fd, &statBuf);
                    int sz = statBuf.st_size;
                    char *mapped = (char *) mmap(NULL, 
                                                sz,
                                                PROT_READ | PROT_WRITE,
                                                MAP_SHARED,
                                                fd,
                                                0);
                    std::cout << mapped << std::endl;
                    close(fd);
                    munmap(mapped, sz);
                    while (++state < 4) {
                        sem_post(semaphore);
                    }
                    while (--state > 3) {
                        sem_wait(semaphore);
                    }  
                }
                if (std::cin.eof()) {
                    while (++state < 4) {
                        sem_post(semaphore);
                    }
                    while (--state > 3) {
                        sem_post(semaphore);
                    }
                    sz = 1;
                    int fd = shm_open(backFile.c_str(), O_RDWR | O_CREAT, accessPerm);
                    CHECK_ERROR(fd, -1, "shm_open");
                    ftruncate(fd, sz);
                    char *mapped = (char *) mmap(NULL, 
                                                sz,
                                                PROT_READ | PROT_WRITE,
                                                MAP_SHARED,
                                                fd,
                                                0);
                    CHECK_ERROR(mapped, MAP_FAILED, "mmap");
                    memset(mapped, '\0', sz);
                    sprintf(mapped, "%c", '\0');
                    close(fd);
                    while (++state < 3) {
                        sem_post(semaphore);
                    }
                    while (--state > 2) {
                        sem_wait(semaphore);
                    }
                    flag = 0;
                }
            }
        }
    }
    return 0;
}
