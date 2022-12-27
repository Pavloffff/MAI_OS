#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <algorithm>

int main(int argc, char const *argv[])
{
    int pipe1fd[2];
	int pipe3fd[2];
	pipe1fd[0] = atoi(argv[0]);
	pipe1fd[1] = atoi(argv[1]);
	pipe3fd[0] = atoi(argv[2]);
	pipe3fd[1] = atoi(argv[3]);
	while (1) {
		std::string str;
        int sz;
		read(pipe1fd[0], &sz, sizeof(sz));
		char *buf = new char[sz];
		read(pipe1fd[0], buf, sz);
        str = buf;
        delete[] buf;
        std::transform(str.begin(),
                       str.end(),
                       str.begin(),
                       [](unsigned char c) { return std::toupper(c); });
		write(pipe3fd[1], &sz, sizeof(sz));
		write(pipe3fd[1], str.c_str(), sz);
	}
    close(pipe1fd[0]);
    close(pipe1fd[1]);
    close(pipe3fd[0]);
    close(pipe3fd[1]);
    return 0;
}