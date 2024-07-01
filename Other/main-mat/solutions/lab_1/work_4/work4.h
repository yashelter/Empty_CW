#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 20

enum input_statements
{
    input_correct,
    input_invalid
};

enum input_statements check_input(int argc, char *argv[], char *flag, bool *is_new_file)
{
    if (argc < 3 || argc > 4)
    {
        return input_invalid;
    }
    if (argv[1][0] != '/' && argv[1][0] != '-')
    {
        return input_invalid;
    }
    if (argc == 3 && (strlen(argv[1]) < 2 || argv[1][1] != 'n'))
    {
        return input_invalid;
    }
    
    *is_new_file = false;
    char chr;

    if (argc == 3)
    {
        *is_new_file = true;
        chr = argv[1][2];
    }
    else
    {
        chr = argv[1][1];
    }

    if (chr != 'i' && chr != 's' && chr != 'a' && chr != 'd')
    {
        return input_invalid;
    }
    *flag = chr;

    return input_correct;
}

enum input_statements try_to_open_r(char *path)
{
    FILE *file = NULL;
    errno_t err;

    err = fopen_s(&file, path, "r");
    if (err != 0) { return input_invalid; }

    fclose(file);
    return input_correct;
}

enum input_statements try_to_open_w(char *path)
{
    FILE *file = NULL;
    errno_t err;

    err = fopen_s(&file, path, "w");
    if (err != 0) { return input_invalid; }

    fclose(file);

    return input_correct;
}

char *create_output_file(char *input_file)
{
    int l = strlen(input_file);
    int out_l = l;
    char *new_path = (char *)malloc(out_l * 2);

    if (new_path == NULL)
    {
        return NULL;
    }
    while (input_file[l] != '\\' && l)
    {
        l--;
    }
    if (l == 0)
    {
        new_path[l] = 'o';
        new_path[l + 1] = 'u';
        new_path[l + 2] = 't';
        new_path[l + 3] = '_';
        for (int i = l + 4; i < out_l + 4; i++)
        {
            new_path[i] = input_file[i - 4];
        }
        new_path[out_l + 4] = '\0';
    }
    else
    {
        for (int i = 0; i < l + 1; i++)
        {
            new_path[i] = input_file[i];
        }
        new_path[l + 1] = 'o';
        new_path[l + 2] = 'u';
        new_path[l + 3] = 't';
        new_path[l + 4] = '_';

        for (int i = l + 5; i < out_l + 4; i++)
        {
            new_path[i] = input_file[i - 4];
        }
        new_path[out_l + 4] = '\0';
    }
    return new_path;
}

enum input_statements flag_d(char *path, char *new_path)
{
    FILE *input_file = NULL;
    FILE *output_file = NULL;

    errno_t err;

    err = fopen_s(&input_file, path, "r");
    if (err != 0) { return input_invalid; }

    err = fopen_s(&output_file, new_path, "w");
    if (err != 0) { return input_invalid; }
   
    int chr;
    while ((chr = fgetc(input_file)) != EOF)
    {
        if (chr >= '0' && chr <= '9')
        {
            continue;
        }
        fprintf(output_file, "%c", chr);
    }
    fclose(input_file);
    fclose(output_file);

    return input_correct;
}

enum input_statements flag_a(char *path, char *new_path)
{
    FILE *input_file = NULL;
    FILE *output_file = NULL;

    errno_t err;

    err = fopen_s(&input_file, path, "r");
    if (err != 0) { return input_invalid; }

    err = fopen_s(&output_file, new_path, "w");
    if (err != 0) { return input_invalid; }

    int ch;
    while ((ch = fgetc(input_file)) != EOF)
    {
        if (ch >= '0' && ch <= '9')
        {
            fprintf(output_file, "%c", ch);
            continue;
        }
        else
        {
            int r;
            int base = 16;
            char buf[MAX_LENGTH];
            char *ptr = buf + MAX_LENGTH - 1;
            *ptr = 0; ptr--;
            int counter = 0;

            while (ch > 0)
            {
                r = ch % base;
                
                if (r > 9){ *ptr = r - 10 + 'A'; }
                else { *ptr = r + '0'; }

                ptr--;
                ch /= base;
                counter++;
            }

            for (int i = MAX_LENGTH - counter - 1; i < MAX_LENGTH - 1; i++)
            {
                fprintf(output_file, "%c", buf[i]);
            }
        }
    }
    fclose(input_file);
    fclose(output_file);

    return input_correct;
}


enum input_statements flag_i(char *path, char *new_path)
{
    FILE *input_file = NULL;
    FILE *output_file = NULL;

    errno_t err;

    err = fopen_s(&input_file, path, "r");
    if (err != 0) { return input_invalid; }

    err = fopen_s(&output_file, new_path, "w");
    if (err != 0) { return input_invalid; }

    unsigned long cnt_str = 1;
    unsigned long cnt_lat = 0;

    int ch;
    while ((ch = fgetc(input_file)) != EOF)
    {
        if (ch == '\n')
        {
            fprintf(output_file, "%lu:  %lu\n", cnt_str, cnt_lat);
            cnt_str++;
            cnt_lat = 0;
            continue;
        }
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            cnt_lat++;
        }
    }
    fprintf(output_file, "%lu:  %lu\n", cnt_str, cnt_lat);

    fclose(input_file);
    fclose(output_file);

    return input_correct;
}


enum input_statements flag_s(char *path, char *new_path)
{
    FILE *input_file = NULL;
    FILE *output_file = NULL;

    errno_t err;

    err = fopen_s(&input_file, path, "r");
    if (err != 0) { return input_invalid; }

    err = fopen_s(&output_file, new_path, "w");
    if (err != 0) { return input_invalid; }

    else
    {
        unsigned long len_str = 1;
        unsigned long cnt = 0;
        int ch;
        while ((ch = fgetc(input_file)) != EOF)
        {
            if (ch == '\n')
            {
                fprintf(output_file, "%lu:  %lu\n", len_str, cnt);
                len_str++;
                cnt = 0;
                continue;
            }
            if (ch != ' ' && (ch < 'A' || ch > 'Z') && (ch < 'a' || ch > 'z') && (ch < '0' || ch > '9'))
            {
                cnt++;
            }
        }
        fprintf(output_file, "%lu:  %lu\n", len_str, cnt);
    }
    fclose(input_file);
    fclose(output_file);

    return input_correct;
}
