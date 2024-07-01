#include "work6.h"
#include <time.h>

void print_hint()
{
    printf("==Program made by Gubarev Mikhail==\n\n");
    printf("=============A few info: ==========\n");
    printf("This program unsuports flags, need only one number epsilon\n");
    printf("Epsilong might be in range (%.18f, 1)\n", MIN_EPS);
    printf("It solves integrales in task\n");
    printf("\n\n");
    printf(">>> Program Output <<<\n\n");
}


int main(int argc, char *argv[])
{
    clock_t start, end;
    double cpu_time_used;
    print_hint();

    start = clock();

    long double epsilon = 0;
    enum input_statements stm = check_input(argc, argv, &epsilon);

    if (stm != input_correct)
    {
        printf("Error : check input");
    }
    else
    {
        printf("Integral a is : %.18Lf\n",
               simpsons_method(epsilon, 0.0L, 1.0L, &integral_a));
        printf("Integral b is : %.18Lf\n",
               simpsons_method(epsilon, 0.0L, 1.0L, &integtral_b));
        printf("Integral c is : %.18Lf\n",
               simpsons_method(epsilon, 0.0L, 1.0L, &integral_c));
        printf("Integral d is : %.18Lf\n",
               simpsons_method(epsilon, 0.0L, 1.0L, &integral_d));
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("\nTime spended : %f seconds\n", cpu_time_used);
    
}