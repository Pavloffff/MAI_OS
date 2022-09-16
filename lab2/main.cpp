#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>

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
        execlp("./child", 
               std::to_string(fd[0][0]).c_str(), 
               std::to_string(fd[0][1]).c_str(),
               std::to_string(fd[1][0]).c_str(),
               std::to_string(fd[1][1]).c_str(),
               std::to_string(fd[2][0]).c_str(),
               std::to_string(fd[2][1]).c_str(),
               NULL);
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
