#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef enum
{
    input_invalid,
    correct,
    file_error,
    copy_error
} statements;

statements check_input(int argc, char *argv[])
{
    if (argc == 4 && strcmp(argv[2], "mask") == 0 && strlen(argv[3]) <= 4)
    {
        return correct;
    }
    if (argc != 3)
    {
        return input_invalid;
    }
    return correct;
}

statements try_open_file(FILE **file, char *path, char *param)
{
    *file = fopen(path, param);
    if (*file == NULL)
    {
        return file_error;
    }
    return correct;
}

void xor8(FILE *file)
{
    char result = 0x00;
    char bytic;
    while (fread(&bytic, sizeof(char), 1, file) == 1)
    {
        result = result ^ bytic;
    }
    printf("Result by summing bytes to modul 2 is : %d\n", result);
}

void xor32(FILE *file)
{
    char result[4], bytes4[4];
    for (int i = 0; i < 4; i++)
    {
        result[i] = 0x00;
    }
    size_t readed = 0;

    while ((readed = fread(bytes4, sizeof(char), 4, file)) > 0)
    {
        if (readed != 4)
        {
            for (int i = (int)readed; i < 4; i++)
            {
                bytes4[i] = 0x00;
            }
        }
        for (int i = 0; i < 4; i++)
        {
            result[i] = result[i] ^ bytes4[i];
        }
    }

    printf("Result by summing (by 4) bytes  to modul 2 is : %X %X %X %X\n", result[0], result[1], result[2], result[3]);
}

bool is_hex(char c)
{
    return ('0' <= c && c <= '9' || 'A' <= c && c <= 'F');
}

void mask_hex(FILE *file, char mask[])
{
    char bytes4[4];
    int cnt = 0;
    size_t readed = 0;

    while ((readed = fread(bytes4, sizeof(char), 4, file)) > 0)
    {
        if (readed != 4)
        {
            for (int i = (int)readed; i < 4; i++)
            {
                bytes4[i] = 0;
            }
        }
        int tcnt = 0;
        for (int i = 0; i < 4; i++)
        {
            // printf("%X & %X == %X", mask[i], bytes4[i], (bytes4[i] & mask[i]))
            if (mask[i] == (bytes4[i] & mask[i]))
            {
                tcnt++;
            }
        }
        if (tcnt == 4)
        {
            cnt++;
        }
    }
    printf("Number of bytes equal to mask is : %d\n", cnt);
}

void print_err(statements stm)
{
    if (stm == input_invalid)
    {
        printf("Error : invalid input\n");
    }
    if (stm == copy_error)
    {
        printf("Error : getted error while copied\n");
    }
    if (stm == file_error)
    {
        printf("Error : while opening file\n");
    }
}

void print_hint()
{
    printf("\t==Program made by Gubarev Mikhail==\n\n");
    printf("\n\n");
    printf("\t  >>>>> Program Output <<<<<\n\n");
}

