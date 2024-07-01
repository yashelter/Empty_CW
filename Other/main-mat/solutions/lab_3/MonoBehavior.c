#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define __USE_XOPEN
#include <time.h>

#define MIN_BUF 200

typedef enum
{
    correct,
    runtime_error,
    invalid_input,
    allocate_error,
    invalid_file,
    end_of,
    not_found,
    nulls, empty
} statements;

double sum(unsigned char *nums, int cnt)
{
    double res = 0.0;
    for (int i = 0; i < cnt; i++)
    {
        res += nums[i];
    }
    return res;
}

double sum_d(unsigned char *nums, int cnt)
{
    return (sum(nums, cnt) / cnt);
}

time_t compare_date(char *str1, char *str2)
{
    struct tm first, second;
    time_t t1, t2;

    strptime(str1, "%d.%m.%Y", &first);
    strptime(str2, "%d.%m.%Y", &second);

    tzset();
    first.tm_sec = 0;
    second.tm_sec = 0;
    first.tm_min = 0;
    second.tm_min = 0;
    first.tm_hour = 0;
    second.tm_hour = 0;
    first.tm_isdst = -1;
    second.tm_isdst = -1;

    t1 = mktime(&first);
    t2 = mktime(&second);

    return t1 - t2;
}


char *read_line(statements *state, FILE *file)
{
    *state = correct;

    int size = 0;
    int buf = 10;
    char *result = (char *)malloc(sizeof(char) * (buf + 1));

    if (result == NULL)
    {
        *state = allocate_error;
        return NULL;
    }
    int ch;
    while ((ch = fgetc(file)) != EOF && ch != '\n')
    {
        size++;
        if (size >= buf)
        {
            buf *= 2;
            char *tmp = (char *)realloc(result, sizeof(char) * (buf + 1));
            if (tmp == NULL)
            {
                free(result);
                *state = allocate_error;
                return NULL;
            }
            result = tmp;
            
        }
        result[size - 1] = (char)ch;
    }
    if (size == 0 && ch == EOF)
    {
        *state = empty;
        free(result);
        return NULL;
    }
    result[size] = '\0';
    if (strcmp(result, "\0") == 0 || strcmp(result, "\r") == 0 || strcmp(result, "\n") == 0)
    {
        *state = invalid_input;
        free(result);
        return NULL;
    }

    return result;
}
