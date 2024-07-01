#include <stdio.h>
#include <math.h>
#include <stdarg.h>

double geometric_middle(int num, ...) {
    if (num == 0) {return 0;}
    va_list args;
    va_start(args, num);
    double result = 1.0L;

    for (int i = 0; i < num - 1; i++) {
        double arg = va_arg(args, double);
        result *= arg;
        //printf("Arg %d: %f\n", i, arg);
    }
    result = pow(result, (1.0 / num));
    va_end(args);

    return result;
}

long double __b_pow_d(long double num, int pow)
{
    if (pow == 1)
    {
        return num;
    }
    if (pow == 0)
    {
        return 1;
    }
    if (pow % 2 == 0)
    {
        long double temp = __b_pow_d(num, pow / 2);
        return temp * temp;
    }
    else
    {
        long double temp = __b_pow_d(num, (pow - 1) / 2);
        return num * temp * temp;
    }
}
long double b_pow_d(long double num, int pow)
{
    if (pow < 0) {
        return 1 / __b_pow_d(num, -pow);
    }
    return __b_pow_d(num, pow);
    
}