#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <algorithm>

int main(int argc, char *argv[])
{
	int pipe1fd[2];
    if (pipe(pipe1fd) == -1) { 
        std::cerr << "pipe1 error" << std::endl; 
        return 1; 
    }
	int pipe2fd[2];
	if (pipe(pipe2fd) == -1) { 
        std::cerr << "pipe2 error" << std::endl;
        return 1; 
    }
    int pipe3fd[2];
	if (pipe(pipe3fd) == -1) { 
        std::cerr << "pipe3 error" << std::endl; 
        return 1; 
    }
	int id1 = fork();
	if (id1 == -1) {
		std::cerr << "fork error" << std::endl;
		return 1;
	} else if (id1 == 0) {
        char *argv1 = new char[sizeof(int)];
        sprintf(argv1, "%d", pipe1fd[0]);
        char *argv2 = new char[sizeof(int)];
        sprintf(argv2, "%d", pipe1fd[1]);
        char *argv3 = new char[sizeof(int)];
        sprintf(argv3, "%d", pipe3fd[0]);
        char *argv4 = new char[sizeof(int)];
        sprintf(argv4, "%d", pipe3fd[1]);
        execlp("./child1", argv1, argv2, argv3, argv4, NULL);
        delete[] argv1;
        delete[] argv2;
        delete[] argv3;
        delete[] argv4;
	} else {
		int id2 = fork();
		if (id2 == -1) {
			std::cerr << "fork error" << std::endl;
			return 1;
		} else  if(id2 == 0){
			char *argv1 = new char[sizeof(int)];
	        sprintf(argv1, "%d", pipe2fd[0]);
	        char *argv2 = new char[sizeof(int)];
	        sprintf(argv2, "%d", pipe2fd[1]);
	        char *argv3 = new char[sizeof(int)];
	        sprintf(argv3, "%d", pipe3fd[0]);
	        char *argv4 = new char[sizeof(int)];
	        sprintf(argv4, "%d", pipe3fd[1]);
	        execlp("./child2", argv1, argv2, argv3, argv4, NULL);
	        delete[] argv1;
	        delete[] argv2;
	        delete[] argv3;
	        delete[] argv4;
		} else {
            std::string str;
            int sz;
            while (getline(std::cin, str)) {
                sz = str.size() + 1;
			    write(pipe1fd[1], &sz, sizeof(sz));
			    write(pipe1fd[1], str.c_str(), sz);
                read(pipe2fd[0], &sz, sizeof(sz));
                char *buf = new char[sz];
                read(pipe2fd[0], buf, sz);
                std::cout << buf << std::endl;
                delete[] buf;
			}
		}
	}
    close(pipe1fd[0]);
    close(pipe1fd[1]);
    close(pipe2fd[0]);
    close(pipe2fd[1]);
    close(pipe3fd[0]);
    close(pipe3fd[1]);
    return 0;
}