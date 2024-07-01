#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MIN_EPS 1e-18

enum input_statements 
{
    input_invalid,
    input_wrong_number,
    input_correct,
    input_incorrect_epsilon,
};

enum input_statements str_to_ld(char *str, long double *res) 
{
    char *result;
    long double number = strtold(str, &result);
    if (*result != '\0') { return input_wrong_number; }
    *res = number;
    return input_correct;
}


long double integral_a(long double eps, long double x) 
{
    if (x == 0.0L) { return logl(1.0L + x) / (x + eps); }
    return logl(1.0L + x) / x;
}

long double integtral_b(long double eps, long double x) 
{
    if (x < eps) { x = 1.0L; }
    return expl((-x * x) * 0.5L);
}

long double integral_c(long double eps, long double x)
 {
    if (x == 1.0L) {  return logl(1.0L / (1.0L - x + eps)); }
    return logl(1.0L / (1.0L - x));
}


long double integral_d(long double eps, long double x) 
{
    if (x < eps) { return powl(x + eps, x + eps); }
    return powl(x, x);
}

long double simpsons_method(long double eps, long double a, long double b, long double(*function)(long double, long double))
{
    int n = 2;
    long double last_val = 0.0L;
    long double curr_val = 2 * eps;

    long double h = 0.0L, f1 = 0.0L, f2 = 0.0L, x_0 = 0.0L;

    while (fabsl(curr_val - last_val) > eps)
    {
        h = (b - a) / n;
        last_val = curr_val;

        curr_val = function(eps, a);
        curr_val += function(eps, b);

        for (int i = 1; i < n; i += 2) {
            x_0 = a + i * h;
            f1 += function(eps, x_0);
        }

        for (int i = 2; i < n - 1; i += 2) {
            x_0 = a + i * h;
            f2 += function(eps, x_0);
        }
        curr_val += 4.0L * f1;
        curr_val += 2.0L * f2;

        curr_val *= (h / 3.0L);

        f1 = 0.0L;
        f2 = 0.0L;

        n *= 2;
    } 

    return curr_val;
}

enum input_statements check_input(int argc, char *argv[], long double* epsilon) 
{
    if (argc != 2) { return input_invalid; } 

    enum input_statements stm = str_to_ld(argv[1], epsilon);
    if (stm != input_correct) { return stm; } 
    
    if (*epsilon < MIN_EPS || *epsilon > 1.0L) { return input_incorrect_epsilon; } 
    return input_correct;
    
}