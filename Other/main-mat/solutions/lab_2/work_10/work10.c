#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


double calculate(int x, int n, double *mas){
    double res = 0;
    for (int i = 0; i < n; i++)
    {
        res *= x;
        res += mas[i];
    }
    return res;
}


void differ(int *n, double **mas)
{
    for (int i = 0; i < *n-1; i++)
    {
        (*mas)[i] = (*mas)[i] * ((*n) - i - 1);
        //printf("%lf \n",(*mas)[i]);
    }
    (*mas)[(*n)-1] = 0;
    *n = (*n) -1;
    
}
double* compute_coefficients(double epsilon, double a, int num, ...) 
{
    double* result_coefficients = (double*) malloc((num) * sizeof(double));
    double* temp = (double*) malloc((num) * sizeof(double));

    if (result_coefficients == NULL) { return NULL; }
    if (temp == NULL) { free(result_coefficients); return NULL; }

    va_list args;
    va_start(args, num);

    for (int i = 0; i < num; i++) {
        temp[i] = va_arg(args, double);
        //printf("%lf ", temp[i]);
    }

    va_end(args);

    // Вычисления коэффициентов многочлена по степеням (x - a)
    int ptr = num;
    double fact_n = 1;
    for (int i = 0; i < num && ptr >= 1; i++) 
    {
        double f_n_a = calculate(a, ptr, temp);

        //printf("%lf ", f_n_a);
        differ(&ptr, &temp); // берём +1 производную
        result_coefficients[ptr] = f_n_a / fact_n;
        fact_n *= (i + 1);
    }
    free(temp);
    return result_coefficients;
}

