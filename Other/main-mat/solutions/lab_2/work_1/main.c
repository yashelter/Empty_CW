#include "work_1.c"

int main(int argc, char *argv[])
{
    char flag;
    statements stm = check_input(argc, argv, &flag);
    if (stm != correct)
    {
        return err_mess(stm);
    }

    if (flag == 'l')
    {
        size_t l;
        stm = strlen_s(argv[2], &l);
        if (stm != correct)
        {
            return err_mess(stm);
        }
        printf("<< Length of string is : %ld\n", l);
    }
    else if (flag == 'r')
    {
        char *new_str;
        stm = get_reversed(argv[2], &new_str);
        if (stm != correct)
        {
            return err_mess(stm);
        }
        printf("<< Reversed string is : %s\n", new_str);
        free(new_str);
    }
    else if (flag == 'u')
    {
        char *new_str;
        stm = operator_u(argv[2], &new_str);
        if (stm != correct)
        {
            return err_mess(stm);
        }
        printf("<< New string is (flag -u) : %s\n", new_str);
        free(new_str);
    }
    else if (flag == 'n')
    {
        char *new_str;
        stm = operator_n(argv[2], &new_str);
        if (stm != correct)
        {
            return err_mess(stm);
        }
        printf("<< New string is (flag -n) : %s\n", new_str);
        free(new_str);
    }
    else if (flag == 'c')
    {
        char *ptr;

        size_t number = strtoll(argv[argc - 1], &ptr, 10);
        if (*ptr != '\0')
        {
            return err_mess(invalid_input);
        }
        char **strings = (char **)malloc(sizeof(char *) * (argc - 3));
        if (strings == NULL)
        {
            return err_mess(memory_error);
        }

        for (int i = 0; i < argc - 3; i++)
        {
            strings[i] = argv[2 + i];
        }
        char *new_str;
        srand(number);

        stm = operator_c(strings, argc - 3, &new_str);
        if (stm != correct)
        {
            free(new_str);
            return err_mess(stm);
        }
        printf("<< New string is (flag -c), with seed <%ld>: %s\n", number, new_str);
        free(new_str);
        free(strings);
    }

    return 0;
}