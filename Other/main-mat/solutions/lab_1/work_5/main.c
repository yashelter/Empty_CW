#include "work5.h"
#include <time.h>

void print_hint()
{
    printf("==Program made by Gubarev Mikhail==\n\n");
    printf("=============A few info: ==========\n");
    printf("This program unsuports flags, so use 2 numbers instead\n");
    printf("The lowest epsilon you can is %0.18f , and the higest %d\n", MIN_EPS, MAX_EPS);
    printf("For number >= 1 can be calculated only (a) and (b) \n");
    printf("Its prits functions values from different options\n\n");
    printf(">>> Program Output <<<\n\n");
}


int main(int argc, char *argv[]) 
{
    clock_t start_time, end_time;
    double cpu_time_used, kk = 0;
    start_time = clock();

    long double x, eps;
    enum input_statements statement = check_input(argc, argv, &x, &eps);
    print_hint();
    switch (statement) 
    {
        case input_invalid:
            printf("Invalid input, please check it");
            break;
        case input_incorrect_epsilon:
            printf("You entered invalid epsilon, please check it");
            break;
        case input_incorrect_number:
            printf("You entered invalid number, please check it");
            break;
        case input_code12:
            printf("Total of (a) : %.18LF\n", summ_a(eps, x));
            printf("Total of (b) : %.18LF\n", summ_b(eps, x));
            printf("Total of (c) : >INF< : diverges\n");
            printf("Total of (d) : >INF< : diverges\n");
            break;
        case input_correct:
            printf("Total of (a) : %.18LF\n", summ_a(eps, x));
            printf("Total of (b) : %.18LF\n", summ_b(eps, x));
            printf("Total of (c) : %.18LF\n", summ_c(eps, x));
            printf("Total of (d) : %.18LF\n", summ_d(eps, x));
            break;
    }
    end_time = clock();
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf(">Time has passed: %0.4f seconds\n", cpu_time_used);
}

