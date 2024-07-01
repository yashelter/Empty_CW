#include "work1.h"

void print_hint()
{
    printf("========Program made by Gubarev Mikhail========\n\n");
    printf("This program is course work number 1, exercise 1 \n");
    printf("====================A few info: ===============\n");
    printf("Each flag need to be with lower register from this list <-h, -p, -s, -e, -a, -f>\n");
    printf("Number should be integer more than zero, non fractional, with less than 19-ns digit\n");
    printf(">-h : Prints natural numbers in limit 100 separable to input\n");
    printf(">-p : Says number is composite or prime\n");
    printf(">-s : Separates number to every digit\n");
    printf(">-e : Prints table of powers to input number: number < 11 \n");
    printf(">-f : Prints factorial to input: number < 20 (because factorial becomes really big) \n");
    
    printf("\n>>> Program Output <<<\n");
}

int main(int argc, char *argv[]) 
{
    print_hint();
    enum inputs_status state =  validate_input_all(argc);

    char *input = argv[2];
    unsigned long long result;

    if (state == inp_wrong || strlen(argv[1]) != 2)
    {
        printf("You did not entered needed values!\n");
        return 0;
    }
    
    enum digit_status status = from_str_to_ll(input, &result);

    switch (status)
    {
    case digit_status_correct:
        //printf("Number: %lld\n", result); // current number
        make_call(argv[1][1], input, result);
        break;
    case digit_status_overflow:
        printf("Error in input: number is too large\n");
        break;
    case digit_status_non_digit:
        printf("Error in input: the string contains non-digit characters\n");
        break;
    case digit_status_fractional:
        printf("Error in input: the string contains a decimal point or comma\n");
        break;
    case digit_status_under_zero:
        printf("Error in input: the number is too small\n");
        break;   
    default:
        printf("Error in input: Unknown status\n");
        break;
    }
}
