
#include <stdio.h>

#define ll long long


typedef enum
{
    input_invalid,
    correct,
    file_error,
    copy_error
} statements;

statements check_input(int argc, char *argv[])
{
    if (argc != 3) {return input_invalid; }
    return correct;
}

statements try_open_file(FILE **file, char *path, char *param)
{
    *file = fopen(path, param);
    if (*file == NULL) { return file_error; }
    return correct;
}

statements copy(FILE *input, FILE *output)
{
    char buffer[8];
    size_t readed_bytes, writed_bytes;

    while (!feof(input))
    {
        readed_bytes = fread(buffer, sizeof(char), sizeof(buffer), input);
        writed_bytes = fwrite(buffer, sizeof(char), readed_bytes, output);
        if (readed_bytes != writed_bytes) { return copy_error; }
    }

    return correct;
}

void print_err(statements stm){
    if (stm == input_invalid){
        printf("Error : invalid input\n");
    }
    if (stm == copy_error){
        printf("Error : getted error while copied\n");
    }
    if (stm == file_error){
        printf("Error : while opening file\n");
    }
}

void print_hint()
{
    printf("\t==Program made by Gubarev Mikhail==\n\n");
    printf("\t============A few info: ===========\n");
    printf("\tThis programm copies file to output file\n");
    printf("\n\n");
    printf("\t  >>>>> Program Output <<<<<\n\n");
}
