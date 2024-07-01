#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#define STR_SIZE 256

typedef enum
{
    correct,
    invalid_file,
    allocate_error,
    invalid_command,
    invalid_index,
    invalid_erange,
    invalid_array,
    int_overflow,
    invalid_format,
    runtime_error,
} statements;

void print_status(statements status)
{
    switch (status)
    {
    case correct:
        printf("Operation completed successfully.\n");
        break;
    case invalid_file:
        fprintf(stderr, "Error: File invalid.\n");
        break;
    case allocate_error:
        fprintf(stderr, "Error: Unable to allocate memory.\n");
        break;
    case invalid_command:
        fprintf(stderr, "Error: Invalid command.\n");
        break;
    case invalid_index:
        fprintf(stderr, "Error: Invalid index.\n");
        break;
    case invalid_erange:
        fprintf(stderr, "Error: Invalid range.\n");
        break;
    case invalid_array:
        fprintf(stderr, "Error: The array is empty.\n");
        break;
    case int_overflow:
        fprintf(stderr, "Error: Integer overflow.\n");
        break;
    case invalid_format:
        fprintf(stderr, "Error: Invalid format.\n");
        break;
    default:
        fprintf(stderr, "Unknown status.\n");
        break;
    }
}

typedef struct Array
{
    char name;
    int *data;
    int length;
    int capacity;
} Array;

void swap_elements(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int max(int a, int b)
{
    return a > b ? a : b;
}
int min(int a, int b)
{
    return a < b ? a : b;
}

statements diff_file(const char *file1, const char *file2)
{
    char *temp1 = NULL;
    char *temp2 = NULL;

    size_t max_path_len = 512;
    temp1 = (char *)malloc(sizeof(char) * max_path_len);
    temp2 = (char *)malloc(sizeof(char) * max_path_len);

    if (temp1 == NULL || temp2 == NULL)
    {
        free(temp1);
        free(temp2);
        return allocate_error;
    }

    if (realpath(file1, temp1) == NULL || realpath(file2, temp2) == NULL)
    {
        free(temp1);
        free(temp2);
        return invalid_file;
    }

    int cmp_result = strcmp(temp1, temp2);

    free(temp1);
    free(temp2);

    if (cmp_result == 0)
    {
        return invalid_file;
    }

    return correct;
}
