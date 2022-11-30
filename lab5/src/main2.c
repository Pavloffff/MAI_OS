#include <stdio.h>
#include <dlfcn.h>
#include "lib.h"

const char* lib1 = "./liblib1.so";
const char* lib2 = "./liblib2.so";

int main(int argc, char const *argv[])
{
    printf("\nWrite:\n [command] [arg1] ... [argN]\n");
    printf("\nIf you want to change methods of calculation, write 0\n");
    printf("\nIf you want to take derivation of f(x) = cos(x), write 1 [point] [delta]\n");
    printf("\nIf you want to calculate number e (base of natural logarithm), write 2 [approximation]\n");
    int command = 0;
    int link = 0;
    void *currentLib = dlopen(lib1, RTLD_LAZY);
    printf("\nCurrent lib is %d\n\n", link);
    float (*Derivative)(float A, float deltaX);
    float (*E)(int x);
    Derivative = (currentLib, "Derivative");
    E = dlsym(currentLib, "E");
    while (scanf("%d", &command) != EOF) {
        switch (command) {
        case 0:
            dlclose(currentLib);
            if (link == 0) {
                currentLib = dlopen(lib2, RTLD_LAZY);
            } else {
                currentLib = dlopen(lib1, RTLD_LAZY);
            }
            link = !link;
            Derivative = dlsym(currentLib, "Derivative");
            E = dlsym(currentLib, "E");
            break;
        
        case 1:
            float A, deltaX;
            scanf("%f%f", &A, &deltaX);
            printf("Answer: %f\n", Derivative(A, deltaX));
            break;
        
        case 2:
            int x;
            scanf("%d", &x);
            printf("Answer: %f\n", E(x));
            break;

        default:
            printf("wrong command\n");
            break;
        }
        printf("\nWrite:\n [command] [arg1] ... [argN]\n");
        printf("\nIf you want to change methods of calculation, write 0\n");
        printf("\nIf you want to take derivation of f(x) = cos(x), write 1 [point] [delta]\n");
        printf("\nIf you want to calculate number e (base of natural logarithm), write 2 [approximation]\n");
        printf("\nCurrent lib is %d\n\n", link);
    }
    return 0;
}
