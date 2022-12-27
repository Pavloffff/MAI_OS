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
    sem_t *semaphore = sem_open(semFile.c_str(),  O_CREAT, accessPerm, 0);
    int state = 0;
    int flag = 1;
    while (flag) {
        int semErrCheck = sem_getvalue(semaphore, &state);
        CHECK_ERROR(semErrCheck, -1, "sem_getvalue");
        if (state == 2) {
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
            if (mapped[0] == '\0') {
                while (++state < 2) {
                    sem_post(semaphore);
                }
                while (--state > 1) {
                    sem_wait(semaphore);
                }
                usleep(00150000);
                munmap(mapped, sz);
                close(fd);                    
                flag = 0;
            }
            std::string str = mapped;
            std::transform(str.begin(),
                           str.end(),
                           str.begin(),
                           [](unsigned char c) { return std::toupper(c); });
            memset(mapped, '\0', sz);
            sprintf(mapped, "%s", str.c_str());
            munmap(mapped, sz);
            close(fd);
            while (++state < 2) {
                sem_post(semaphore);
            }
            while (--state > 1) {
                sem_wait(semaphore);
            }
            usleep(00150000);
        }
    }
    return 0;
}
