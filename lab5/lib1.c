#include <stdio.h>
#include <math.h>

float Derivative(float A, float deltaX)
{
    printf("\nCalculation of derivative function f(x) = cos(x)\n");
    printf("in point %f with approximation %f\n", A, deltaX);
    printf("by formula f'(x) = (f(A + deltaX) – f(A))/deltaX\n");
    float dfdx = (cos(A + deltaX) - cos(A)) / deltaX;
    return dfdx;
}

float binPow(float x, int y)
{
    float z = 1.0;
    while (y > 0) {
        if (y % 2 != 0) {
            z *= x;
        }
        x *= x;
        y /= 2;
    }
    return z;
}

float E(int x)
{
    printf("\nCalculation value of number e (base of natural logarithm)\n");
    printf("with approximation %d\n", x);
    printf("by formula e(x) = (1 + 1/x) ^ x\n");
    float mant = 1 + (1 / (float) x);
    float e = binPow(mant, x);
    return e;
}