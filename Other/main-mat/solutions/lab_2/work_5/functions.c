#include <math.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <ctype.h>

#define ll long long

typedef enum
{
    runtime_error,
    correct,
    memory_error,
    invalid_input,
    nah_executed,

} statements;

statements validate_int(int num, int range, int end)
{
    return (num < range || num > end || num >= INT_MAX || num <= INT_MIN) ? invalid_input : correct;
}

typedef struct
{
    char *symbol;
    int value;
} RomanDigit;

statements to_roman(int num, char ***result)
{
    statements stm = validate_int(num, -1000000, 1000000);
    if (stm != correct)
    {
        return stm;
    }

    RomanDigit romans[14] = {{"X̅", 10000}, {"M", 1000}, {"CM", 900}, {"D", 500}, {"CD", 400}, {"C", 100}, {"XC", 90}, {"L", 50}, {"XL", 40}, {"X", 10}, {"IX", 9}, {"V", 5}, {"IV", 4}, {"I", 1}};

    int buffer_size = 4, real_size = 0, points = 0;
    **result = (char *)malloc(sizeof(char) * buffer_size);

    if (*result == NULL)
    {
        return memory_error;
    }

    if (num < 0)
    {
        (**result)[points] = '-';
        num *= -1;
        real_size++;
        points++;
    }
    if (num == 0)
    {
       (**result)[points++] = 'N';
       (**result)[points++] = 'a';
       (**result)[points++] = 'n';
       (**result)[points++] = '\0';
        return correct;
    }

    for (int i = 0; num > 0; i++)
    {
        while (num >= romans[i].value)
        {
            real_size += strlen(romans[i].symbol);
            if (buffer_size < real_size + 1)
            {
                char *new_buf = (char *)realloc(**result, buffer_size * 2);
                if (new_buf == NULL)
                {
                    free(*result);
                    return memory_error;
                }
                buffer_size *= 2;
                **result = new_buf;
            }
            (**result)[points++] = romans[i].symbol[0];
            if (strlen(romans[i].symbol) == 2){
                (**result)[points++] = romans[i].symbol[1];
            }
            num -= romans[i].value;
        }
    }
    (**result)[points++] = '\0';
    return correct;
}

unsigned int *get_fib(unsigned int limit, int *l)
{
    int buffer_size = 2, i = 2;
    unsigned int *mas = (unsigned int *)malloc(sizeof(unsigned int) * buffer_size);
    if (mas == NULL)
    {
        return NULL;
    }

    mas[0] = 1;
    mas[1] = 1;

    while (mas[i - 1] < limit)
    {
        unsigned int c = mas[i - 2] + mas[i - 1];

        if (c >= UINT_MAX / 2)
        {
            free(mas);
            return NULL;
        }

        if (buffer_size <= i)
        {
            buffer_size *= 2;
            unsigned int *temp = (unsigned int *)realloc(mas, sizeof(unsigned int) * buffer_size);
            if (temp == NULL)
            {
                free(mas);
                return NULL;
            }
            mas = temp;
        }
        mas[i] = c;
        i++;
    }
    *l = i;
    return mas;
}

statements zeckendorf_look(unsigned int number, char **result)
{
    if (number >= UINT_MAX / 2)
    {
        return invalid_input;
    }

    int n;
    unsigned int *fubs = get_fib(number, &n);
    if (fubs == NULL)
    {
        return invalid_input;
    }

    *result = (char *)malloc(sizeof(char) * (n + 2));
    //printf("%d", n);
    for (int i = 0; i < n + 1; i++)
    {
        (*result)[i] = '0';
    }

    if (*result == NULL)
    {
        free(fubs);
        return memory_error;
    }

    int i = n - 1;
    int ptr = n - 1;
    while (number != 0)
    {
        if (number >= fubs[i])
        {
            number = number - fubs[i];
            (*result)[ptr] = '1';
        }
        i--;
        ptr--;
        if (i < 0)
        {
            free(fubs);
            return runtime_error;
        }
    }
    free(fubs);
    (*result)[n] = '1';
    (*result)[n + 1] = '\0';
    return correct;
}

int num_len_cc(int num, int base){
    int cnt = 0;
    while (num > 0)
    {
        num /= base;
        cnt++;
    }
    return cnt;
}
statements _10th_to_cc(int num, int base, char **result, bool upcase)
{
    statements stm = validate_int(num, -1e5,  1e5);

    if (stm != correct) { return stm; }
    if (base < 2 || base > 36) { base = 10; }
    if (num == 0) 
    { 
        *result = (char *) malloc(sizeof(char) * 2);
         if (*result == NULL) { return memory_error; }
        (*result)[0] = '0';
        (*result)[1] = '\0';

        return correct;
    }
    bool under_zero = false;
    if (num < 0)
    {
        under_zero = true;
        num = num * -1;
    }
    int buffer_size = num_len_cc(num, base) + under_zero;
    *result = (char *) malloc(sizeof(char) * (buffer_size + 1));
    if (*result == NULL)
    {
        return memory_error;
    }

    int ptr = buffer_size - 1;


    while (num > 0)
    {
        int digit = num % base;
        (*result)[ptr] = upcase ? ((digit > 9) ? digit - 10 + 'A' : digit + '0'):
                                   (digit > 9) ? digit - 10 + 'a' : digit + '0';
        ptr--;
        num /= base;
    }
    if (under_zero) { (*result)[0] = '-'; }
    (*result)[buffer_size]  = '\0';

    return correct;
}

statements cc_to_10th(char *str, int base, long long *result, bool upcase)
{
    int len = strlen(str);
    if (len > 8) return invalid_input;
    if (base < 2 || base > 36) base = 10;
    
    *result = 0;
    bool sign = false;

    if (str[0] == '-') { sign = true; }

    for (int i = (sign ? 1: 0); i < len; i++)
    {
        *result = (*result) * base;
        char c = str[i];
        int current = (upcase) ? ((c >= '0' && c <= '9') ? c - '0' : c - 'A' + 10) :
                                 ((c >= '0' && c <= '9') ? c - '0' : c - 'a' + 10);
        if (current > base + 1) { return invalid_input; } 

        *result = (*result) + current;
    }
    if (sign)
    {
        *result = (*result) * -1;
    }
    return correct;
}

statements print_memory_dump(const void *value, int size, char **res)
{
    const unsigned char *bytes = (const unsigned char *)value;

    int buffer_size = (sizeof(char) * (size * 9));
    int chars_written = 0;
    *res = (char *) malloc(buffer_size);

    for (int i = size-1; i >=0; i--)
    {
        for (int bit = 7; bit >= 0; bit--)
        {
            int bit_value = (bytes[i] >> bit) & 1;
            chars_written += snprintf(*res + chars_written, buffer_size - chars_written, "%d", bit_value);
        }
        chars_written += snprintf(*res + chars_written, buffer_size - chars_written, "%c", ' ');
    }
    return correct;
}

bool not_digit(char s)
{
    s = tolower(s);
    return  0 <= (s - 'a') && (s-'a') >= 26;
}