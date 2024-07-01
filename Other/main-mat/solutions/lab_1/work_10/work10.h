#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LENGTH 20

enum input_statements
{
    input_correct,
    input_wrong_base,
    input_invalid,
    input_wrong_length
};


enum input_statements check_base(int base)
{
    if (2 > base || base > 36) { return input_wrong_base; }
    return input_correct;
}

int char_digit_to_number(char digit)
{
    if ('0' <= digit && digit <= '9') { return digit - '0'; }
    if ('A' <= digit && digit <= 'Z') {  return digit - 'A' + 10; }
    if ('a' <= digit && digit <= 'z') {  return digit - 'a' + 10; }
    return -1; 
}


enum input_statements ss_to_base_10(char* str, int base, long long *result) 
{
    long long number = 0;
    long long remain_part = 1;

    long long sign = 1;
    int lwst = 0;
    
    if (str[0] == '-')  { sign = -1;  lwst = 1; }

    if (strlen(str) >= MAX_LENGTH) {return input_wrong_length; } 

    for (int i = strlen(str) - 1; i >= lwst; i--) 
    {
        int num = char_digit_to_number(str[i]);
        if (num == -1 || num > base - 1) { return input_invalid; }

        number = number + num * remain_part;
        remain_part *= base;
    }

    number *= sign;
    *result = number;

    return input_correct;
}

int get_len(int n, int base)
{
    if(n == 0) { return 1;}
    int res = 0;
    while(n > 0)
    {
        res++;
        n /= base;
    }
    return res;
}

enum input_statements ll_to_cc(long long number, int base, char *result)
{
    int r, sign = 1, len = 0;
    if (number < 0)
    {
        sign = -1;
        number = -number;
        len ++;;
    }

    if (number != 0) { len += get_len(number, base); }
    else {result[0] = '0'; result[1] = '\0'; return input_correct;}
    
    //if (len >= MAX_LENGTH) {return input_wrong_length; }
    
    char *ptr = result + len;
    *ptr = '\0';
    ptr--;

    while (number > 0)
    {
        r = number % base;
        *ptr = (r > 9) ? r - 10 + 'A' :r + '0';
        ptr--;
        number /= base;
        
    }
    if (sign == -1) { *ptr = '-'; }

    return input_correct;

}
