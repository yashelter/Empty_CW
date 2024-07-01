#include <stdio.h>
#include <math.h>
#include <stdlib.h>


double ex_1(double x)
{
    return x * x - 16;
}

double ex_2(double x)
{
    return sin(x);
}

double ex_3(double x)
{
    return powf(2.72, x) - 2.72;
}


double dichotomy(double a, double b, double (*function)(double), double eps)
{
    double mid = 0;
    while (fabs(b - a) > fabs(eps))
    {
        mid = (a + b) / 2;
        if (function(mid) >= -fabs(eps)  &&  function(mid) <= fabs(eps)) 
        {
            return mid;
        } 
        if (function(mid) * function(a) < fabs(eps)) 
        {
            b = mid;
        } 
        else 
        {
            a = mid;
        }
    }
    return mid;
}
