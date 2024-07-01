#include "work7.h"

void print_hint(){
    printf("==Program made by Gubarev Mikhail==\n\n");
    printf("=============A few info: ==========\n");
    printf(">-r :\n ");
    printf("Write to a file, the path of which is provided as the last argument of the command line,\n");
    printf("tokens and files file1 and file2, the paths of which are provided as the third and\n");
    printf("fourth command line arguments, respectively. Tokens from file1 are on odd positions,\n");
    printf("and from file2 on even positions. Tokens in the input files are separated by an\n");
    printf("arbitrary number of space characters, tabs, and line breaks. If the number of tokens\n");
    printf("in the files is different, after reaching the end of one of the input files, it is\n");
    printf("necessary to write the remaining tokens from the other input file to the output file.\n");
    printf("Tokens in the output file should be separated by a single space character.\n\n");
    printf(">-a :\n");
    printf("Write to a file, the path of which is provided as the last argument of the command line,\n");
    printf("a file whose path is provided as the third command line argument,\n");
    printf("in such a way that:\n");
    printf("\t-- in every tenth token, first, all Latin alphabet letters are converted to their lowercase equivalent,\n");
    printf("and then all characters are converted to their equivalent ASCII codes written in base 4;\n");
    printf("\t-- in every second (and not tenth) token, all Latin alphabet letters are converted to their lowercase equivalent;\n");
    printf("\t-- in every fifth (and not tenth) token, all characters are converted to their equivalent ASCII codes written\n");
    printf("in base 8.\n");
    printf("\n>>> Program Output<<<\n");
}

int main(int argc, char *argv[])
{
    print_hint();
    char flag;
    enum base_statements input_state = check_input_validate_n_run(argc, argv, &flag);

    switch (input_state)
    {
    case base_error_in_function:
        printf("Error: exception in a function, check files please\n");
        break;
     case base_invalid_input:
        printf("Error: invalid input\n");
        break;
    case base_too_long_string:
        printf("Error: was too long string\n");
        break;
    case base_correct:
        printf("All might be Okey!\n");
        break;
    default:
        break;
    }
    
    return 0;
}