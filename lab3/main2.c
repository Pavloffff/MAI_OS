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
            printf("%.2lf ", matrix[i][j]);
        }
        printf("\n");
    }
}

int cntOfThreadsForMinors = 0;
double *firstRow;
typedef struct
{
    double **matrix;
    int n;
    int cols;
    int i;
} threadArgs;

void clearMinor(double **matrix, int n)
{
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
        matrix[i] = NULL;
    }
    free(matrix);
    matrix = NULL;
}

double getMinor(double **matrix, int n)
{
    if (n == 1) {
        return matrix[0][0];
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
            res += sign * matrix[0][i] * getMinor(mr, n - 1);
            sign *= (-1);
            clearMinor(mr, n - 1);
        }
        return res;
    }
}

void *routine(void *args)
{
    threadArgs *arg = (threadArgs *) args;
    double **matrix = arg->matrix;
    int n = arg->n, i = arg->i, cols = arg->cols;
    if (cntOfThreadsForMinors == 0) {    
        for (int j = i; j < i + cols && j < n; j++) {
            double **mr = malloc(sizeof(double *) * (n - 1));
            for (int it1 = 0; it1 < n - 1; it1++) {
                mr[it1] = malloc(sizeof(double) * (n - 1));
            }
            for (int it1 = 1; it1 < n; it1++) {
                for (int it2 = 0; it2 < n; it2++) {
                    if (it2 == j) {
                        continue;
                    } else if (it2 < j) {
                        mr[it1 - 1][it2] = matrix[it1][it2];
                    } else {
                        mr[it1 - 1][it2 - 1] = matrix[it1][it2];
                    }
                }
            }
            printf("\nminor of thread %d:\n", i);
            print(mr, n - 1);
            firstRow[j] = getMinor(mr, n - 1);
            clearMinor(mr, n - 1);
        }
        free(arg);
        return NULL;
    } else {
        return NULL;
    }
}

int main(int argc, char const *argv[])
{
    const long CORES = sysconf(_SC_NPROCESSORS_ONLN);
    if (argc != 3) {
        printf("Syntax error: expected ./*executable_file_name* Square_matrix_dim Number_of_threads\n");
        exit(1);
    }
    int n = atoi(argv[1]), cntOfThreads = atoi(argv[2]);
    if (n <= 1) {
        printf("Math error: this is no matrix\n");
        exit(1);
    }
    if (cntOfThreads > CORES) {
        printf("Core error: in this device %ld logic cores\n", CORES);
        exit(1);
    }
    firstRow = (double *) calloc(n, sizeof(double));
    pthread_t *threads = (pthread_t *) calloc(cntOfThreads, sizeof(double));
    if (threads == NULL) {
        printf("Allocation error: can't allocate array of threads\n");
        exit(1);
    }
    double **matrix = malloc(sizeof(double *) * n);
    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(sizeof(double) * n);
    }
    if (matrix == NULL) {
        printf("Allocation error: can't allocate exeptet matrix\n");
    }
    printf("Enter the square matrix dim of %d:\n", n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%lf", &matrix[i][j]);
        }
    }
    if (cntOfThreads > n) {
        cntOfThreadsForMinors = n - cntOfThreads;
        cntOfThreads = n;
    }
    int colsForThread = n / cntOfThreads;
    int colsMod = n % cntOfThreads;
    //printf("%.2lf\n", getMinor(matrix, n));
    for (int i = 0; i < cntOfThreads; i++) {
        threadArgs *args = malloc(sizeof(threadArgs));
        args->matrix = matrix;
        args->n = n;
        if (i == cntOfThreads - 1) {
            args->cols = colsForThread + colsMod;
        } else {
            args->cols = colsForThread;
        }
        args->i = i;
        if (pthread_create(threads + i, NULL, routine, args) != 0) {
            printf("Thread creation error\n");
            exit(1);
        }   
    }
    for (int i = 0; i < cntOfThreads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Thread join error");
            exit(1);
        }
    }
    double det = 0;
    for (int i = 0; i < n; i++) {
        if (i % 2 != 0) {
            firstRow[i] *= -1;
        }
        det += firstRow[i] * matrix[0][i];
    }
    printf("%.2lf\n", det);
    clearMinor(matrix, n);
    free(firstRow);
    firstRow = NULL;
    free(threads);
    threads = NULL;
    return 0;
}
