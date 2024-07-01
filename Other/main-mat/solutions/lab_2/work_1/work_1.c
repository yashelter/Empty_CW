#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum
{
    correct,
    memory_error,
    parameters_error,
    invalid_input,
} statements;

bool is_lower_altha_s(char c)
{
    return 'a' <= c && c <= 'z';
}

bool is_altha_s(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool is_digit_s(char c)
{
    return ('1' <= c && c <= '9');
}

char to_upper(char c)
{
    if (!is_lower_altha_s(c))
    {
        return c;
    }
    return c - 'a' + 'A';
}

statements strlen_s(char *string, size_t *len)
{
    if (string == NULL)
    {
        return parameters_error;
    }

    *len = 0;

    while (string[*len] != '\0')
    {
        (*len)++;
    }
    return correct;
}

// need free after usage
statements get_reversed(char *string, char **result)
{
    if (string == NULL)
    {
        return parameters_error;
    }

    size_t l;
    statements stm = strlen_s(string, &l);
    if (stm != correct)
    {
        return stm;
    }

    *result = (char *)malloc(sizeof(char) * (l + 1));
    if (*result == NULL)
    {
        return memory_error;
    }

    for (int i = l - 1; i > -1; i--)
    {
        (*result)[l - 1 - i] = string[i];
    }
    (*result)[l] = '\0';

    return correct;
}

statements operator_u(char *string, char **result)
{
    if (string == NULL)
    {
        return parameters_error;
    }

    size_t l;
    statements stm = strlen_s(string, &l);
    if (stm != correct)
    {
        return stm;
    }

    // if (result != NULL) { free(result); } // ?

    *result = (char *)malloc(sizeof(char) * (l + 1));
    if (*result == NULL)
    {
        return memory_error;
    }

    for (int i = 1; i < l + 1; i++)
    {
        if (i % 2 == 0)
        {

            (*result)[i - 1] = to_upper(string[i - 1]);
        }
        else
        {
            (*result)[i - 1] = string[i - 1];
        }
    }
    (*result)[l] = '\0';

    return correct;
}

statements operator_n(char *string, char **result)
{
    if (string == NULL)
    {
        return parameters_error;
    }

    size_t l;
    statements stm = strlen_s(string, &l);
    if (stm != correct)
    {
        return stm;
    }

    *result = (char *)malloc(sizeof(char) * (l + 1));
    if (*result == NULL)
    {
        return memory_error;
    }
    int point = 0;

    for (int i = 0; i < l; i++)
    {
        if (is_digit_s(string[i]))
        {
            (*result)[point++] = string[i];
        }
    }
    for (int i = 0; i < l; i++)
    {
        if (is_altha_s(string[i]))
        {
            (*result)[point++] = string[i];
        }
    }
    for (int i = 0; i < l; i++)
    {
        if (!is_digit_s(string[i]) && !is_altha_s(string[i]))
        {
            (*result)[point++] = string[i];
        }
    }

    return correct;
}

void swap_c(char **s1, char **s2)
{
    char *temp = *s1;
    *s1 = *s2;
    *s2 = temp;
}

statements operator_c(char *strings[], int size_s, char **result)
{
    if (strings == NULL)
    {
        return parameters_error;
    }

    size_t total_l = 0;

    // shuffle mas to get random order

    for (int i = 0; i < size_s && size_s != 1; i++)
    {
        int j = rand() % 4;
        swap_c(&strings[i], &strings[j]);
    }

    for (int i = 0; i < size_s; i++)
    {
        size_t l;
        statements stm = strlen_s(strings[i], &l);
        if (stm != correct)
        {
            return stm;
        }
        total_l += l;
    }

    *result = (char *)malloc(sizeof(char) * (total_l + 1));

    if (*result == NULL)
    {
        return memory_error;
    }

    int point = 0;
    for (int i = 0; i < size_s; i++)
    {
        size_t mj = 0;
        statements stm = strlen_s(strings[i], &mj);
        if (stm != correct)
        {
            free(*result);
            return stm;
        }

        for (size_t j = 0; j < mj; j++)
        {
            (*result)[point++] = strings[i][j];
        }
    }

    return correct;
}

int strcmp_s(const char *str1, const char *str2)
{
    while (*str1 != '\0' && *str2 != '\0')
    {
        if (*str1 != *str2)
        {
            return 0;
        }
        str1++;
        str2++;
    }

    if (*str1 != '\0' || *str2 != '\0')
    {
        return 0;
    }

    return 1;
}

statements check_input(int argc, char *argv[], char *flag)
{
    if (argc < 3)
    {
        return invalid_input;
    }
    char *flagy = argv[1];
    if (!strcmp_s(flagy, "-l") && !strcmp_s(flagy, "-r") && !strcmp_s(flagy, "-u") && !strcmp_s(flagy, "-n") && !strcmp_s(flagy, "-c"))
    {
        return invalid_input;
    }
    *flag = flagy[1];

    if (strcmp_s(flagy, "-c") && argc < 4)
    {
        return invalid_input;
    }

    return correct;
}

int err_mess(statements stm)
{
    if (stm == invalid_input)
    {
        printf("Error : invalid input\n");
    }
    else if (stm == memory_error)
    {
        printf("Error : memory error\n");
    }
    else if (stm == parameters_error)
    {
        printf("Error : invalid parameters\n");
    }

    return 0;
}