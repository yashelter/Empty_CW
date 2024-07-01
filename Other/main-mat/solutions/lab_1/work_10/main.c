#include "work10.h"


void print_hint()
{
    printf("==Program made by Gubarev Mikhail==\n\n");
    printf("=============A few info: ==========\n");
    printf("This program unsuports flags, it uses real-time input\n");
    printf("First numbers is base and must be in the range [2..36]\n");
    printf("Others must have len less then %d in base 9 \n", MAX_LENGTH);
    printf("\n\n");
    printf(">>> Program Output <<<\n\n");
}



int main()
{    
    print_hint();

    int base;

    printf("Please enter the base for numbers: \n>>> ");
    scanf_s("%d", &base, MAX_LENGTH);

    enum input_statements statement = check_base(base);

    if (statement != input_correct){
        printf("Error: invalid input, please re-check it\n");
        return 0;
    }

    char input[MAX_LENGTH];
    char * input_ptr = input;
    long long max_number = 0;
    bool was_numbers = false;

    printf("Waiting numbers, \"q\" or \"Stop\" to exit: \n >>> ");

    int checkpoint = scanf_s("%s", input, MAX_LENGTH);

    while (strcmp(input, "Stop") != 0 && strcmp(input, "q") != 0)
    {
        long long current_number = 0;
        statement = ss_to_base_10(input, base, &current_number);

        if (statement != input_correct || checkpoint != 1)
        {
            printf("You entered something wrong, please check and input again\n >>> ");
            scanf_s("%s", input, MAX_LENGTH);
            continue;
        }

        if (llabs(current_number) > llabs(max_number)) { max_number = current_number; }
        
        printf(" >>> ");
        checkpoint = scanf_s("%s", input, MAX_LENGTH);
        was_numbers = true;
        
    }

    if (!was_numbers) 
    {
        printf("Numbers were uncorrect, or you quit too early\n");
        return 0;
    }
   

    printf("\n\nThe maximum number in abs() from list is (in 10CC): %lld\n\n", max_number);
    for (int i = 9; i < 37; i += 9)
    {
        char str_num_in_cc[MAX_LENGTH];
        char *ptr = str_num_in_cc;
        enum input_statements num_st = ll_to_cc(max_number, i, ptr);
        if (num_st == input_correct) { printf("<< This number in base %d is : %s\n", i, ptr);}
        else { printf("<< Cant get number in base %d \n", i);}
    }
    
    return 0;
}
// how to use input.txt >> .\main.exe < input.txt << in cmd, powershell doesnt works