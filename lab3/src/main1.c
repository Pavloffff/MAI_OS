#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

double **matrix = NULL;

typedef struct
{
    int rows;
    int n;
    int i;
} threadArgs;

void print(double **matrix, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%.2lf ", matrix[i][j]);
        }
        printf("\n");
    }
}

void clearMinor(double **matrix, int n)
{
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
        matrix[i] = NULL;
    }
    free(matrix);
    matrix = NULL;
}

void *routine(void *args)
{
    threadArgs *arg = (threadArgs *) args;
    int n = arg->n, i = arg->i, rows = arg->rows;
    for (int k = 0; k < i + rows; k++) {
        for (int j = k + 1; j < n; j++) {
            double del = (-1) * matrix[j][k] / matrix[k][k];
            for (int x = 0; x < n; x++) {
                matrix[j][x] += matrix[k][x] * del;
            }
        }
    }
    free(args);
    return NULL;
}

double det(double **matrix, int n)
{
    double res = 1;
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            double del = (-1) * matrix[j][i] / matrix[i][i];
            for (int k = 0; k < n; k++) {
                matrix[j][k] += matrix[i][k] * del;
            }
        }
    }
    for (int i = 0; i < n; i++) {
        res *= matrix[i][i];
    }
    return res;
}

int main(int argc, char const *argv[])
{
    if (argc < 3 || argc > 4) {
        printf("Syntax error: expected ./*executable_file_name* Square_matrix_dim Number_of_threads\n");
        printf("or ./*executable_file_name* Square_matrix_dim Number_of_threads -t\n");
        exit(1);
    }
    int n = atoi(argv[1]), cntOfThreads = atoi(argv[2]);
    if (cntOfThreads > n - 1) {
        printf("Error: Number_of_threads must be less then Square_matrix_dim\n");
        exit(1);
    }
    pthread_t *threads = (pthread_t *) calloc(cntOfThreads, sizeof(double));
    if (threads == NULL) {
        printf("Allocation error: can't allocate array of threads\n");
        exit(1);
    }
    matrix = malloc(sizeof(double *) * n);
    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(sizeof(double) * n);
    }
    if (matrix == NULL) {
        printf("Allocation error: can't allocate exeptet matrix\n");
    }
    if (argc == 3) {
        printf("Enter the square matrix dim of %d:\n", n);
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%lf", &matrix[i][j]);
        }
    }
    FILE *timeTest;
    clock_t begin, end;
    if (argc == 4) {
        timeTest = fopen("../benchmark/outp1", "a");
        begin = clock();
    }
    int rowsForThread = (n - 1) / cntOfThreads;
    int rowsMod = (n - 1) % cntOfThreads;
    for (int i = 0; i < cntOfThreads; i++) {
        threadArgs *args = malloc(sizeof(threadArgs));
        args->n = n;
        if (i == cntOfThreads - 1) {
            args->rows = rowsForThread + rowsMod;
        } else {
            args->rows = rowsForThread;
        }
        args->i = i;
        if (pthread_create(threads + i, NULL, routine, args) != 0) {
            printf("Thread creation error\n");
            exit(1);
        } 
    }
    for (int i = 0; i < cntOfThreads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Thread join error\n");
            exit(1);
        }
    }
    double res = 1;
    for (int i = 0; i < n; i++) {
        res *= matrix[i][i];
    }
    if (argc == 3) {
        printf("%.2lf\n", res);
    }
    if (argc == 4) {
        end = clock();
        fprintf(timeTest, "%lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
        fclose(timeTest);
    }
    clearMinor(matrix, n);
    free(threads);
    threads = NULL;
    return 0;
}
