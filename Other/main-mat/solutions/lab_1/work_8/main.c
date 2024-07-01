#include "work8.h"

void print_hint()
{
    printf("==Program made by Gubarev Mikhail==\n\n");
    printf("=============A few info: ==========\n");
    printf("This program unsuports flags\n");
    printf("Instead use two parameters : path to input, and output files\n");
    printf("In file numbers must have len equal or less then %d\n", MAX_LENGTH);
    printf("Numbers in the wrong format will be ignored\n");
    printf("\n\n");
    printf(">>> Program Output <<<\n\n");
}


int main(int argc, char* argv[])
{
    print_hint();

    FILE *input = NULL, *output = NULL;
    enum input_statements state = check_input(argc, argv, &input, &output);

    if (state != input_correct)
    {
         printf("Error : please check input parameters\n");
         close_opened_file(input);
         close_opened_file(output);
    }
    else
    {
        state = cycle(input, output);

        if (state != input_correct)
        {
            printf("Error : Check numbers in file, something incorrect \n");
        }
        else 
        {
            printf("State : All done without errors \n");
        }
        close_opened_file(input);
        close_opened_file(output);
    }
       
    return 0;


}