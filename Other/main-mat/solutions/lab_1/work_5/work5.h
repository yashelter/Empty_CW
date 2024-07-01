#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define MIN_EPS 2e-18
#define MAX_EPS 1
#define MAX_ITERATIONS 1e9 // bit more then 1 sec

#define MAX_X 2e18
#define MIN_X 1



enum input_statements
{
    input_invalid,
    input_incorrect_epsilon,
    input_correct,
    input_incorrect_number,
    input_code12
};


bool char_to_ld(char *str, long double **number)
{
    char *result;
    **number = strtold(str, &result);
    
    if (*result != '\0') { return false; }
    return true;
}


enum input_statements check_input(int argc, char *argv[],  long double *x, long double *epsilon)
{
    bool st1 = char_to_ld(argv[2], &epsilon),
         st2 = char_to_ld(argv[1], &x);

    if (argc != 3 || !st1 || !st2) {  return input_invalid; } 

    if (*epsilon < MIN_EPS || *epsilon > MAX_EPS) { return input_incorrect_epsilon; } 
    if (*x > MAX_X ) {return input_incorrect_number; }
    if (*x >= 1) {return input_code12;}
    return input_correct;
    
}


long double summ_a(long double eps, long double x)
 {
    long double value = x, n = 1;
    long double function = x + 1, d_func = eps + 1;

    while (value > eps)
    {
        n++;
        value = (value * x) / n;
        function += value;
        if (n > MAX_ITERATIONS) { break;}
    }
    return function;
}

long double summ_b(long double eps, long double x) 
{
    long long n = 0;
    long double function = 1.0L;
    long double value = 1.0L;

    while (fabsl(value) > eps) 
    {
        value = ((value * x * x ) * (-1.0L)) / (4 * n * n + 6 * n + 2) ;
        function += value;
        n++;
        if (n > MAX_ITERATIONS) { break;}
    }
    return function;
}


long double summ_c(long double eps, long double x) 
{
    long long n = -1;
    long double function = 1.0L;
    long double value = 1.0L, last_val = 0;

    while (fabsl(value - last_val) > eps) 
    {
        n++;
        last_val = value; 
        value = value * (9 * n * n * x * x + 18 * n * x * x + 9 * x * x) / (9 * n * n + 9 * n + 2);
        function += value;

        if (n > MAX_ITERATIONS) { break;}
    }
    return function;
}


long double summ_d(long double eps, long double x) {

    long long n = 0;
    long double function = 0.0L;
    long double value = 0.0L, last_val = 1.0L;

    while (fabsl(last_val - value) > eps) 
    {
        n++;
        value = last_val;
        last_val = value * (-1.0L * x * x * (2 * n - 1 ) / (2 * n));
        function += last_val;

        if (n > MAX_ITERATIONS) { break;}
    }
    return function;
}


