#ifndef __LIB1_H__
#define __LIB1_H__

// ggame@ggame:~/OS/lab5$ gcc -c -Wall -Werror -fpic lib1.c
// ggame@ggame:~/OS/lab5$ gcc -shared -o liblib1.so lib1.o
// ggame@ggame:~/OS/lab5$ gcc -L. -Wall -o main1 main1.c -llib1
// /usr/bin/ld: ./liblib1.so: неопределённая ссылка на «cos»
// collect2: error: ld returned 1 exit status
// ggame@ggame:~/OS/lab5$ gcc -L. -Wall -o main1 main1.c -llib1 -lm
// ggame@ggame:~/OS/lab5$ ./main1
// ./main1: error while loading shared libraries: liblib1.so: cannot open shared object file: No such file or directory
// ggame@ggame:~/OS/lab5$ pwd
// /home/ggame/OS/lab5
// ggame@ggame:~/OS/lab5$ LD_LIBRARY_PATH=/home/ggame/OS/lab5:$LD_LIBRARY_PATH
// ggame@ggame:~/OS/lab5$ ls
// lib1.c  lib1.h  lib1.o  lib2.c  lib2.h  liblib1.so  main1  main1.c  main.c  Makefile  t1
// ggame@ggame:~/OS/lab5$ ./main1

extern float Derivative(float A, float deltaX);
extern float E(int x);

#endif