#ifndef __LIB1_H__
#define __LIB1_H__

// ggame@ggame:~/OS/lab5$ gcc -c -Wall -Werror -fpic lib2.c
// ggame@ggame:~/OS/lab5$ gcc -shared -o liblib2.so lib2.o
// ggame@ggame:~/OS/lab5$ gcc -L. -Wall -o main2 main1.c -llib2 -lm
// ggame@ggame:~/OS/lab5$ ./main2


extern float Derivative(float A, float deltaX);
extern float E(int x);

#endif