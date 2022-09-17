#include <iostream>
#include <unistd.h>
#include <string>
#include <algorithm>

int main(int argc, char const *argv[])
{
    int fd[3][2];
    fd[0][0] = std::stoi(argv[0]);
    fd[0][1] = std::stoi(argv[1]);
    fd[1][0] = std::stoi(argv[2]);
    fd[1][1] = std::stoi(argv[3]);
    fd[2][0] = std::stoi(argv[4]);
    fd[2][1] = std::stoi(argv[5]);
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
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            close(fd[i][j]);
        }
    }
    return 0;
}
