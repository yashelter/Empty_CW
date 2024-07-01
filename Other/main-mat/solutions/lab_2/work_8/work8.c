#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h> // для length
#include <time.h>
#include <ctype.h>

#define ll long long 

typedef enum
{
    memory_error,
    correct,
    runtime_error,
} statements;

char *format_num(char *str)
{
    int ind = 0;
    ll l = strlen(str);

    while (str[ind] == '0') ind++;
    if (ind == l)
    {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    for (int i = 0; i < l - ind; i++)
    {
        str[i] = str[i + ind];
        
    }
    str[l - ind] = '\0';

    return str;
}

void move_right_char(char **str, int l)
{
    for (int i = l - 1; i >= 1; i--)
    {
        (*str)[i] = (*str)[i-1];
    }
}

int get_num(char d)
{
    return isdigit(d) ? d - '0' : tolower(d) + 10 - 'a';
}

ll max(ll a, ll b )
{
    if (a > b) {return a;}
    return b;
}

char *sum_num(statements **stm, int base, char *a, char *b)
{
    ll l1 = strlen(a);
    ll l2 = strlen(b);

    ll max_l = max(l1, l2);

    char *result = (char *) malloc(sizeof(char) * (max_l + 1));
    ll buffer = 0;

    for (int ptr = 1; max_l - ptr >= 0; ptr++)
    {
        int first = 0, second = 0;
        if (l1 - ptr >= 0){
            first = get_num(a[l1 - ptr]);
        }
        if (l2 - ptr >= 0){
             second = get_num(b[l2 - ptr]);
        }

        int local_res = first + second + buffer;
        if (local_res % base > 9)
        {
            result[max_l - ptr] = (local_res % base) + 'A' - 10;
        }
        else
        {
             result[max_l - ptr] = (local_res % base) + '0';
        }

        buffer = local_res / base;
    }

    if (buffer > 0)
    {
        char *new_result = (char *)realloc(result, sizeof(char) * (max_l + 2));

        if (new_result == NULL)
        {
            free(result);
            **stm = memory_error;
            return NULL;
        }
        result = new_result;
        move_right_char(&result, max_l + 1);

        result[0] = (buffer < 10) ? buffer + '0' : buffer - 10 + 'A';
        max_l++;
    }

    result[max_l] = '\0';
    **stm = correct;
    free(a);
    
    return result;
}

char *sum_nums(statements *stm, int base, int count, ...)
{
    *stm = correct;

    char *answer = (char *)malloc(sizeof(char) * 2);
    if (answer == NULL)
    {
        *stm = memory_error;
        return NULL; 
    }

    answer[0] = '0';
    answer[1] = '\0';

    va_list ptr;
    va_start(ptr, count);

    for (int i = 0; i < count; ++i)
    {
        char *num = va_arg(ptr, char *);

        answer = sum_num(&stm, base, answer, num);
        if (*stm != correct)
        {
            free(answer);
            return NULL;
        }
    }

    va_end(ptr);

    format_num(answer);
    return answer;
}