#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>


#define NUM  50000
#define ONE 1.0L
#define EPS_MIN 0.01
#define EPS_MAX 0.00000001
#define SECOND 1e9

enum input_status_code{
    input_invalid,
    input_correct
};


long double char_to_ld(char *str)
{
    char *result;
    long double number = strtold(str, &result);
    if (*result != '\0') {return 0; }
    return number;
}

bool is_correct_number(char* str)
{
    long double number = char_to_ld(str);
    if(number > EPS_MIN || number < EPS_MAX || number == 0) {return false;}
    return true;
}


long double bin_pow(long double x, long long pow) 
{
    if (pow == 0) { return 1; }
    if (pow % 2 == 0) 
    {
        long double result = bin_pow(x, pow / 2) ;
        return result * result;
    }
    else
    {
        long double result = bin_pow(x, (pow - 1) / 2) ;
        return x *  result * result;
    }
}

long long numbers[NUM];
long double simple[NUM];


void eratosphen(long long* mass)
{
    for(long long i = 2; i <= NUM; i++)
    {
        if(mass[i] > 0){ continue; }
        for(long long j = 2; i * j <= NUM; j++) { mass[i*j] = 1;}
    }
}


void precalc()
{
    for(int i = 0; i < NUM; i++) { numbers[i] = 0; }
    for(int i = 0; i < NUM; i++) { simple[i] = 0.0L;}

    eratosphen(numbers);
    int n = 0;
    for(int i = 0; i < NUM; i++)
    {
        if(numbers[i] == 0){ simple[n] = i; n++;}
    }
}



// limits 
long double e_limit(char* str)
{
    long double epsilon = char_to_ld(str);

    long long n = 2;
    long double drob = ONE / n;

    while (drob > epsilon)
    {
        n++;
        drob = ONE / n;
    }
    long double answer = bin_pow((ONE + ONE / n), n);
    return answer;
}

long double pi_limit(char* str)
{
    char *result;
    long long number = strtol(str, &result, 10);
    long double srav = powl(ONE * 10, (-1) * (number + 9));

    long long n = 1;
    long double pi = ONE;
    long double multiplayer = ONE * 4;
    while(fabsl(pi - multiplayer) > srav)
    {
        pi = multiplayer;
        multiplayer *= (ONE * 4 * n * n + 4 * n) / ((ONE * 2 * n + 1) *( ONE * 2 * n + 1));
        n++;
    }
    return pi;
}


long double ln2_limit(char* str)
{
    char *result;
    long long number = strtol(str, &result, 10);
    long double srav = powl(ONE * 10, (-1) * (number + 12));

    long long n = 1;
    long double ln_2 = ONE * 2;
    long double multiplayer = ONE;
    while(fabsl(ln_2 - multiplayer) > srav)
    {
        ln_2 = multiplayer;
        n++;
        multiplayer = n * (powl(ONE * 2, (ONE/n)) - 1);

    }
    return ln_2;
}

long double sqrt2_limit(char* str)
{
    char *result;
    long long number = strtol(str, &result, 10);
    long double srav = powl(ONE * 10, (-1) * (number + 1));

    long double x_1 = 0L;
    long double x_2 = -0.5L;

    while(fabsl(x_2 - x_1) > srav)
    {
        x_1 = x_2;
        x_2 = x_1 - (x_1 * x_1 / 2) + 1;
    }
    return x_1;
}


long double gamma_limit(char* str)
{
    char *result;
    long long number = strtol(str, &result, 10);
    long double srav = powl(ONE * 10, (-1) * (number + 1));
    long double answer = 0L;
    long long n = 1;
    long double drob = ONE;

    while(drob > srav)
    {
        n++;
        answer += drob;
        drob = ONE / n;
    }
    return answer - logl(n);
}



// tailor rows


long double e_tailor(char* str)
{
    char *result;
    long long number = strtol(str, &result, 10);
    long double srav = powl(ONE * 10, (-1)*(number+1));
    long long n = 1;
    long long fact = 1;
    long double row_summ = 0L;
    while(ONE/fact > srav){
        row_summ += ONE / fact;
        n++;
        fact *= n;
    }
    return ONE + row_summ;
}


long double pi_tailor(char* str)
{
    long double epsilon = char_to_ld(str);

    long double answer = -ONE;
    long long n = 1;
    long double summ = -ONE;
    long double one = ONE;

    while(fabsl(summ) > epsilon)
    {
        n++;
        summ = one / (ONE * 2 * n - 1);
        answer += summ;
        one = -one;
    }
    return -ONE * 4 * answer;
}

long double ln2_tailor(char* str)
{
    long double epsilon = char_to_ld(str);

    long double answer = ONE;
    long long n = 1;
    long double summ = ONE;
    long double one = -ONE;

    while(fabsl(summ) > epsilon)
    {
        n++;
        summ = one / n;
        answer += summ;
        one = -one;
    }
    return answer;
}


long double sqrt2_tailor(char* str)
{
    long double epsilon = char_to_ld(str);
    long double power = ONE / (ONE * 4);

    long double answer = powl(2, power);

    while(power > epsilon)
    {
        answer *= powl(2, power);
        power *= 0.5L ;
    }
    return answer;
}


long double gamma_tailor(char* str)
{
    long long koefs[100];
    for(int i = 0; i < 100; i++) { koefs[i] = i*i; }

    long double pi = 4.0 * atanl(1.0);

    long double epsilon = char_to_ld(str) / 100;

    long long k = 2;
    int n = 2;
    
    long double first_drob = ONE;
    long double second_drob = ONE / 2;
    long double answer = 0.0L;

    while(fabsl(first_drob - second_drob) > epsilon){
        if (k == koefs[n]) { k++; n++; }

        first_drob = powl(floorl(sqrtl(k)),-2);
        second_drob = ONE / k;

        answer += first_drob;
        answer -=  second_drob;
        k++;
    }
    return answer - ((pi*pi) / (ONE * 6)) ;
}


// by equations

long double func_e(long double x) { return log(x) - ONE; }

double df_e(double x) {
    return 1.0 / x;
}

long double e_newton(char* str)
{
    long double epsilon = char_to_ld(str);

    long double x = ONE * 2;
    long double fx = ONE * 3;

    while (fabs(fx) > epsilon) 
    {
        fx = func_e(x);
        long double dfx = df_e(x);
        x = x - fx / dfx;
    }
    return x;
}



long double func_pi(long double x) { return cosl(x) + ONE; }

long double df_pi(long double x){ return -sinl(x); }

long double pi_newton(char* str)
{
    long double x = ONE * 3;
    long double fx = ONE * 4;

    long double epsilon = char_to_ld(str);
    while(fabsl(fx) > epsilon)
    {
        fx = func_pi(x);
        long double dfx = df_pi(x);

        x = x - fx / dfx;
    }
    return x;
}



long double ln2_f(long double x) { return expl(x) - 2.0L; }

long double ln2_df(long double x) { return expl(x); }

long double ln2_newton(char* str)
{
    long double epsilon = char_to_ld(str);
    long double x = ONE / 2, fx = ONE;

    while (fabs(fx) > epsilon) 
    {
        fx = ln2_f(x);
        long double dfx = ln2_df(x);
        x = x - fx / dfx;
    }
    return x;
}


long double sqrt2_func(long double x) { return x*x - 2; }

long double sqrt2_df(long double x) { return 2 * x; }


long double sqrt2_newton(char* str)
{
    long double epsilon = char_to_ld(str);

    long double x = ONE;

    while (fabsl(sqrt2_func(x)) > epsilon) 
    {
        x = x - sqrt2_func(x) / sqrt2_df(x);
    }
    return x;

}

void gamma_limit_for_eq(char* str, long double *limit)
{
    long double epsilon = char_to_ld(str);

    long long t = 2;

    long double answer = ONE;
    long double up = 0;
    long double down = ONE;

    while(fabsl(up - down) > epsilon)
    {
        down = up;
        up =  (simple[t] - 1.0L)/(simple[t]);
        answer *= up;
        t++;
        if (t >= NUM) { return; }
    }
    *limit = answer * logl(simple[t]);
}


long double gamma_f(long double x, long double limit) { return expl(-x) - limit;}

long double gamma_fd(long double x) { return -expl(-x); }


long double gamma_dirihle(char* str)
{
    long double limit;
    gamma_limit_for_eq(str, &limit);

    long long epsilon = char_to_ld(str);
    long double x_1 = 0.5L;
    long double x_2 = x_1 - (gamma_f(x_1, limit) / gamma_fd(x_1));
    int t;

    while(fabsl(x_2 - x_1) > epsilon)
    {
        x_1 = x_2;
        x_2 = x_1 - (gamma_f(x_1, limit) / gamma_fd(x_1) );
        t++;
        if (t > 10 * SECOND) {break;}
    }
    return x_1;
}

enum input_status_code check_input(int argc, char** argv)
{
    if(argc == 2 && is_correct_number(argv[1])) { return input_correct;}
    return input_invalid;
    
}