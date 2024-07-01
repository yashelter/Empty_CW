#include <stdio.h>
#include "work2.h"

void print_hint()
{
    printf("==Program made by Gubarev Mikhail==\n\n");
    printf("=============A few info: ==========\n");
    printf("This program unsuports flags, so use number instead\n");
    printf("The lowest you can is %f, and the higest %f\n", EPS_MAX, EPS_MIN);
    printf("Its prits functions values from different options\n\n");
    printf(">>> Program Output <<<\n\n");
}

int main(int argc, char** argv)
{
    print_hint();
    precalc();

    switch (check_input(argc, argv)){
        case input_correct:

            printf("============= E ==========\n\n");
            printf("E by limit: %.9Lf\n", e_limit(argv[1]));
            printf("E by Tailor: %.9Lf\n", e_tailor(argv[1]));
            printf("E by equation: %.9Lf\n\n", e_newton(argv[1]));

            printf("============= PI ==========\n\n");
            printf("PI by limit: %.9Lf\n", pi_limit(argv[1]));
            printf("PI by Tailor: %.9Lf\n", pi_tailor(argv[1]));
            printf("PI by equation: %.9Lf\n\n", pi_newton(argv[1]));

            printf("============= ln(2) ==========\n\n");
            printf("ln(2) by limit: %.9Lf\n", ln2_limit(argv[1]));
            printf("ln(2) by Tailor: %.9Lf\n", ln2_tailor(argv[1]));
            printf("ln(2) by equation: %.9Lf\n\n", ln2_newton(argv[1]));

            printf("============= sqrt(2) ==========\n\n");
            printf("sqrt(2) by limit: %.9Lf\n", sqrt2_limit(argv[1]));
            printf("sqrt(2)by Tailor: %.9Lf\n", sqrt2_tailor(argv[1]));
            printf("sqrt(2)by equation: %.9Lf\n\n", sqrt2_newton(argv[1]));

            printf("============= gamma ==========\n\n");
            printf("gamma by limit: %.9Lf\n", gamma_limit(argv[1]));
            printf("gamma by row: %.9Lf\n", gamma_tailor(argv[1]));
            printf("gamma by equation: %.9Lf\n\n", gamma_dirihle(argv[1]));
            break;

        case input_invalid:
            printf("Invalid input, please check for correctivitily");
            break;
    }
    return 0;
}