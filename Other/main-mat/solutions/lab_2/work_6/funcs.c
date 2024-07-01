#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define ll long long


typedef enum
{
    invalid_input,
    correct,
    memory_problem,
    nah_executed
} statements;


typedef struct
{
    char symbol;
    int value;
} RomanDigit;

bool not_digit(char s){
    s = tolower(s);
    return  0 <= (s - 'a') && (s-'a') >= 26;
}

statements from_roman(char *str, int **result) 
{
    int len = strlen(str);
    if (len > 26) return invalid_input;

    if (strcmp(str, "Nan") == 0)
    {
        **result = 0;
        return correct;
    }

  RomanDigit romans[8] = {
    { 'I', 1 },
    { 'V', 5 },
    { 'X', 10 },
    { 'L', 50 },
    { 'C', 100 },
    { 'D', 500 },
    { 'M', 1000 },
    //{ 'X̅', 10000 }
    };

    **result = 0;
    int prev = 0;

    for (int i = len - 1; i >= 0; i--) 
    {
        int curr = -1;
        for (int j = 0; j < 8; j++)
        {
            if (romans[j].symbol == str[i])
            {
                curr = romans[j].value;
                break;
            }
        }
        
        if (curr == -1) 
        {
            **result = -1;
            return invalid_input;
        }
        if (curr < prev)  { **result = **result - curr; } 
        else   { **result = **result + curr; }
        prev = curr;
    }
    return correct;
}

statements zeckendorf_look_int(char *str, unsigned int **res)
{
    int len = strlen(str);

    if (len >= 21 || len < 2 || str[len-1] != '1')
    {
        return invalid_input;
    }

    **res = 0;

    int prev = 0, curr = 1;

    for (int i = 0; i < len-1; i++)
    {
        if (str[i] == '1'){ **res += curr;}

        int temp = curr;
        curr = prev + curr;
        prev = temp;
    }
    return correct;
}



statements ss_to_base_10(char* str, int base, int **result, bool isUpcase) 
{
    int len = strlen(str);
    if (len > 10) return invalid_input; // на больших не спасает, но нет смысла отслеживать

    if (base < 2 || base > 36) base = 10;
    
    **result = 0;
    bool sign = false;

    if (str[0] == '-') { sign = true; }

    for (int i = (sign ? 1: 0); i < len; i++)
    {
        **result = (**result) * base;
        char c = str[i];
        int current = (isUpcase) ? ((c >= '0' && c <= '9') ? c - '0' : c - 'A' + 10) :
                                   ((c >= '0' && c <= '9') ? c - '0' : c - 'a' + 10);
        if (current > base + 1) { return invalid_input; } 

        **result = (**result) + current;
    }
    if (sign)
    {
        **result = (**result) * -1;
    }
    return correct;
}