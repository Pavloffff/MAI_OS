#include <stdio.h>
#include <math.h>

float Derivative(float A, float deltaX)
{
    printf("\nCalculation of derivative function f(x) = cos(x)\n");
    printf("in point %f with approximation %f\n", A, deltaX);
    printf("by formula f'(x) = (f(A + deltaX) – f(A-deltaX))/(2*deltaX)\n");
    float dfdx = (cos(A + deltaX) - cos(A - deltaX)) / (2 * deltaX);
    return dfdx;
}

int prodTree(int l, int r)
{
    if (l > r) {
        return 1;
    }
    if (l == r) {
        return l;
    }
    if (l - r == 1) {
        return l * r;
    }
    int m = (l + r) / 2;
    return prodTree(l, m) * prodTree(m + 1, r);
}

int fact(int n)
{
    if (n < 0) {
        return 0;
    }
    if (n == 0) {
        return 1;
    }
    if (n == 1 || n == 2) {
        return n;
    }
    return prodTree(2, n);
}

float machineEpsilon(void)
{
	float e = 1.0f;
	while (1.0f + e / 2.0f > 1.0f)
		e /= 2.0f;
	return e;
}

float E(int x)
{
    printf("\nCalculation value of number e (base of natural logarithm)\n");
    printf("with approximation %d\n", x);
    printf("by sum of row by n from 0 to x f(n) = (1/(n!))\n");
    float e = 0;
    for (int n = 0; n <= x; n++) {
        float tmp = ((float) 1 / fact(n));
        if (fabs(tmp) <= machineEpsilon()) {
            printf("Approximation can not work because of mashine Epsilon of float is %.8f\n", machineEpsilon());
            break;
        }
        e += tmp;
    }
    return e;
}