#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    char *file_name = (char *)calloc(1, sizeof(char));
    int size = 0;
    char c;
    while ((c = getchar()) != '\n') {
        file_name[size] = c;
        file_name = (char *)realloc(file_name, (++size + 1) * sizeof(char));
    }
    file_name[size++] = '\0';
    printf("%s\n", file_name);
    free(file_name);
    return 0;
}
