#include "work3.h"

void print_hint(){
    printf("==Program made by Gubarev Mikhail==\n\n");
    printf("=============A few info: ==========\n");
    printf(">-q : Solves quadratic equation; enter floating: eps > 0, a > 0, b > 0, c > 0\n");
    printf(">-m : Says separates the first number to second or not; enter natural: a > 0, b > 0\n");
    printf(">-t : Says can it be rectangular triange or not; enter floating: eps > 0, a > 0, b > 0, c > 0\n\n");
    printf(">>> Program Output <<<\n");
}
int main(int argc, char* argv[])
{
    print_hint();
    char flag;
    enum input_checkmark input_state = check_input_validate_n_run(argc, argv, &flag);

    switch (input_state)
    {
    case input_correct:
        // printf("All good\n");
        break;
    case input_invalid_input:
        printf("Error: Invalid Input, please check it\n");
        return 0;
        break;
    case input_invalid_flag:
        printf("Error: Invalid <Flag>, please check it\n");
        return 0;
        break;
    case input_invalid_cnt_param:
        printf("Error: Invalid <count> of params for your flag, please check it\n");
        return 0;
        break;
    case invalid_param:
        printf("Error: One of params not in limits, please check it\n");
        return 0;
        break;
    default:
        break;
    }


    switch (flag)
    {
        case 'q':
            print_quadratic(argc, argv);
            break;

        case 'm':
            unsigned long long num1, num2;
            enum digit_status nums_state = get_ll_numbers(argc, argv, &num1, &num2);
            bool key = check_for_separable(num1, num2);
            if (key) { printf("First second is separable to Second number\n"); }
            else { printf("First second is not separable to Second number\n");}
            break;

        case 't':
            long double eps, n1, n2, n3;
            get_fractional(argv[2], &eps);
            get_fractional(argv[3], &n1);
            get_fractional(argv[4], &n2);
            get_fractional(argv[5], &n3);
            bool checkp = is_triangle(eps, n1, n2, n3);
            if (checkp){ printf("This is rectangular triangle\n"); }
            else {printf("This is not rectangular triangle\n");}
            break;
        default:
            break;
    }
    return 0;
}