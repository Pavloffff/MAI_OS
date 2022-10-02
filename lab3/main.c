#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

void print(double **matrix, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%lf ", matrix[i][j]);
        }
        printf("\n");
    }
}

double det(double **matrix, int n)
{
    int i = 0;
    if (n == 1) {
        return matrix[0][0];
    } else if (n == 2) {
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    } else {
        double res = 0, sign = 1;
        for (int i = 0; i < n; i++) {
            double **mr = malloc(sizeof(double *) * (n - 1));
            for (int k = 0; k < n - 1; k++) {
                mr[k] = malloc(sizeof(double) * (n - 1));
            }
            for (int j = 1; j < n; j++) {
                for (int k = 0; k < n; k++) {
                    if (k == i) {
                        continue;
                    } else if (k < i) {
                        mr[j - 1][k] = matrix[j][k];
                    } else {
                        mr[j - 1][k - 1] = matrix[j][k];
                    }
                }
            }
            res += sign * matrix[0][i] * det(mr, n - 1);
            sign *= (-1);
            for (int i = 0; i < n - 1; i++) {
                free(mr[i]);
                mr[i] = NULL;
            }
            free(mr);
            mr = NULL;
        }
        return res; 
    }
}

int main(int argc, char const *argv[])
{
    int n = 0;
    scanf("%d", &n);
    double **matrix = malloc(sizeof(double *) * n);
    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(sizeof(double) * n);
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%lf", &matrix[i][j]);
        }
    }
    
    printf("%.2lf\n", det(matrix, n));

    for (int i = 0; i < n; i++) {
        free(matrix[i]);
        matrix[i] = NULL;
    }
    free(matrix);
    matrix = NULL;
    return 0;
}
