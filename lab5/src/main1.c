#include <stdio.h>
#include "lib.h"

int main(int argc, char const *argv[])
{
    printf("\nWrite:\n [command] [arg1] ... [argN]\n");
    printf("\nIf you want to take derivation of f(x) = cos(x), write 1 [point] [delta]\n");
    printf("\nIf you want to calculate number e (base of natural logarithm), write 2 [approximation]\n\n");
    int command = 0;
    while (scanf("%d", &command) != EOF) {
        switch (command) {
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
        printf("\nIf you want to take derivation of f(x) = cos(x), write 1 [point] [delta]\n");
        printf("\nIf you want to calculate number e (base of natural logarithm), write 2 [approximation]\n\n");
    }
    return 0;
}
