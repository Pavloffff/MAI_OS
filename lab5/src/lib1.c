#include <stdio.h>

const float PI = 3.1415926;

float Cos(float x)
{
    int y = 100;
    int div = (int) (x / PI);
    x = x - (div * PI);
    char sign = 1;
    if (div % 2 != 0) {
        sign = -1;
    }
    float result = 1.0;
    float inter = 1.0;
    float num = x * x;
    for (int i = 1; i <= y; i++) {
        float comp = 2.0 * i;
        float den = comp * (comp - 1.0);
        inter *= num / den;
        if (i % 2 == 0) {
            result += inter;
        } else {
            result -= inter;
        }
    }
    return sign * result;
}

float Derivative(float A, float deltaX)
{
    printf("\nCalculation of derivative function f(x) = Cos(x)\n");
    printf("in point %f with approximation %f\n", A, deltaX);
    printf("by formula f'(x) = (f(A + deltaX) – f(A))/deltaX\n");
    printf("cos(A) = %f\n", Cos(A));
    float dfdx = (Cos(A + deltaX) - Cos(A)) / deltaX;
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
    float mant = (float) 1 + ((float) 1 / (float) x);
    float e = binPow(mant, x);
    return e;
}
