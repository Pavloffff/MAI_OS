#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char const *argv[])
{
    int fd[3][2];
    for (int i = 0; i < 3; i++) {
        if (pipe(fd[i]) == -1) {
            std::cerr << "pipe error\n";
            return -1;
        }
    }
    pid_t Child1Id = fork();
    if (Child1Id == -1) {
        std::cerr << "fork error\n";
        return -1;
    } else if (Child1Id == 0) {
        pid_t Child2Id = fork();
        if (Child2Id == -1) {
            std::cerr << "fork error\n";
            return -1;
        } else if (Child2Id == 0) {
            while (1) {
                fflush(stdout);
                std::string str;
                int sz;
                read(fd[1][0], &sz, sizeof(sz));
                char *buf = new char[sz];
                read(fd[1][0], buf, sz);
                str = buf;
                delete[] buf;
                std::transform(str.begin(),
                               str.end(),
                               str.begin(),
                               [](unsigned char c) { return (c == ' ') ? '_' : c; });
                write(fd[2][1], &sz, sizeof(sz));
                write(fd[2][1], str.c_str(), sz);
            }
        } else {
            while (1) {
                fflush(stdout);
                std::string str;
                int sz;
                read(fd[0][0], &sz, sizeof(sz));
                char *buf = new char[sz];
                read(fd[0][0], buf, sz);
                str = buf;
                delete[] buf;
                std::transform(str.begin(),
                               str.end(),
                               str.begin(),
                               [](unsigned char c) { return std::toupper(c); });
                write(fd[1][1], &sz, sizeof(sz));
                write(fd[1][1], str.c_str(), sz);
            }
        }
    } else {
        std::string str;
        int sz;
        while (getline(std::cin, str)) {
            fflush(stdout);
            sz = str.size() + 1;
            write(fd[0][1], &sz, sizeof(sz));
            write(fd[0][1], str.c_str(), sz);
            read(fd[2][0], &sz, sizeof(sz));
            char *buf = new char[sz];
            read(fd[2][0], buf, sz);
            str = buf;
            delete[] buf;
            std::cout << str << "\n";
        }
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                close(fd[i][j]);
            }
        }
    }
    return 0;
}
