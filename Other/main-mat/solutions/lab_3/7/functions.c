#include "Liver.c"


statements parsing_input_data(char *filename, list *lst)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return invalid_file;
    }
    int q = fgetc(file);
    if (q == EOF)
    {
        fclose(file);
        return invalid_file;
    }
    ungetc(q, file);

    statements st = correct;
    char *input_string = NULL;

    while ((input_string = read_line(&st, file)) != NULL)
    {
        char *tmp = (char *)malloc(sizeof(char) * (strlen(input_string) + 1));
        if (tmp == NULL)
        {
            free(input_string);
            fclose(file);
            return allocate_error;
        }
        
        strcpy(tmp, input_string);

        int cnt_tokens = count_tokens(tmp, " ");
        free(tmp);

        if (cnt_tokens != 5 && cnt_tokens != 6)
        {
            fclose(file);
            free(input_string);
            return invalid_input;
        }
        char *surname = strtok(input_string, " ");
        char *name = strtok(NULL, " ");
        char *patronymic = NULL; // отчество

        if (cnt_tokens == 6)
        {
            patronymic = strtok(NULL, " ");
            if (patronymic == NULL)
            {
                fclose(file);
                free(input_string);
                return invalid_input;
            }
        }
        char *birthday = strtok(NULL, " ");
        char *gender = strtok(NULL, " ");
        char *income = strtok(NULL, " ");

        if (surname == NULL || name == NULL || birthday == NULL || gender == NULL || income == NULL)
        {
            fclose(file);
            free(input_string);
            return invalid_input;
        }

        int len_surname = (int)strlen(surname);
        int len_name = (int)strlen(name);
        int len_patronymic = patronymic == NULL ? 0 : (int)strlen(patronymic);
        int len_birth = (int)strlen(birthday);

        if (strlen(gender) > 1)
        {
            fclose(file);
            free(input_string);
            return invalid_input;
        }

        char gender_char = gender[0];

        if (gender_char != 'W' && gender_char != 'M')
        {
            fclose(file);
            free(input_string);
            return invalid_input;
        }

        int len_income = (int)strlen(income);
        if (len_income > 20)
        {
            fclose(file);
            free(input_string);
            return invalid_input;
        }
        char *surname_line = (char *)malloc(sizeof(char) * (len_surname + 1));

        if (surname_line == NULL)
        {
            free(input_string);
            fclose(file);
            return allocate_error;
        }
        char *name_line = (char *)malloc(sizeof(char) * (len_name + 1));

        if (name_line == NULL)
        {
            free(input_string);
            free(surname_line);
            fclose(file);
            return allocate_error;
        }
        char *birthday_line = (char *)malloc(sizeof(char) * (len_birth + 1));
        if (birthday_line == NULL)
        {
            free(surname_line);
            free(name_line);
            free(birthday_line);
            fclose(file);
            free(input_string);
            return allocate_error;
        }
        char *income_line = (char *)malloc(sizeof(char) * (len_income + 1));
        if (income_line == NULL)
        {
            free(surname_line);
            free(name_line);
            free(birthday_line);
            free(income_line);
            fclose(file);
            free(input_string);
            return allocate_error;
        }

        char *patronymic_line = patronymic == NULL ? NULL : (char *)malloc(sizeof(char) * (len_patronymic + 1));

        if (patronymic_line == NULL && patronymic != NULL)
        {
            free(surname_line);
            free(name_line);
            patronymic_line = NULL;
            fclose(file);
            free(input_string);
            return allocate_error;
        }

        strcpy(surname_line, surname);
        strcpy(name_line, name);

        if (patronymic != NULL)
        {
            strcpy(patronymic_line, patronymic);
        }
        strcpy(birthday_line, birthday);
        strcpy(income_line, income);
          
        struct tm in;
        if (strptime(birthday_line, "%d.%m.%Y", &in) == NULL || in.tm_year > 123 || in.tm_year < 10)
        {
            free(surname_line);
            free(name_line);
            free(birthday_line);
            free(income_line);
            if (patronymic_line != NULL)
            {
                free(patronymic_line);
            }
            patronymic_line = NULL;
            fclose(file);
            free(input_string);
            return invalid_input;
        }

        if (!is_latin_str(surname_line) || !is_latin_str(name_line) || (patronymic_line != NULL && !is_latin_str(patronymic_line)))
        {
            free(surname_line);
            free(name_line);
            free(birthday_line);
            free(income_line);
            if (patronymic_line != NULL)
            {
                free(patronymic_line);
            }
            patronymic_line = NULL;
            fclose(file);
            free(input_string);
            return invalid_input;
        }

        Liver *new_liver = (Liver *)malloc(sizeof(Liver));
        if (new_liver == NULL)
        {
            free(surname_line);
            free(name_line);
            free(birthday_line);
            free(income_line);
            if (patronymic_line != NULL)
            {
                free(patronymic_line);
            }
            fclose(file);
            free(input_string);
            return allocate_error;
        }

        char *end;
        long double average_income = strtold(income_line, &end);
        if (*end != '\0' && *end != '\r' && *end != '\n' || end == income_line ||
            (average_income == 0.0 && errno == ERANGE) ||
            average_income >= HUGE_VAL - 10 || average_income <= -HUGE_VAL + 10 || average_income < 0 || income_line[0] == '-')
        {
            free(surname_line);
            free(name_line);
            free(birthday_line);
            free(income_line);
            if (patronymic_line != NULL)
            {
                free(patronymic_line);
            }
            fclose(file);
            free(input_string);
            free(new_liver);
            return invalid_input;
        }

        new_liver->surname = surname_line;
        new_liver->name = name_line;
        new_liver->patronymic = patronymic_line;
        new_liver->birth_day = birthday_line;
        new_liver->gender = gender_char;
        new_liver->average_income = average_income;

        // print_liver(new_liver);
        node *temp = find_same_liver(lst, new_liver);
        if (temp != NULL && lst->in_head != NULL)
        {
            fclose(file);
            free(surname_line);
            free(name_line);
            free(birthday_line);
            free(income_line);
            if (patronymic != NULL && patronymic_line != NULL)
            {
                free(patronymic_line);
            }
            free(input_string);
            free(new_liver);
            return invalid_input;
        }

        st = add_node(lst, new_liver);
        if (st != correct)
        {
            fclose(file);
            free(surname_line);
            free(name_line);
            free(birthday_line);
            free(income_line);
            if (patronymic != NULL && patronymic_line != NULL)
            {
                free(patronymic_line);
            }
            free(input_string);
            free(new_liver);
            return st;
        }

        free(income_line);
        free(input_string);
    }
    fclose(file);
    return correct;
}

void print_list(list *lst, FILE *file)
{
    if (lst->in_head == NULL)
    {
        printf("the list is empty!\n");
    }

    node *tmp = lst->in_head;
    while (tmp != NULL)
    {
        print_liver(tmp->liver, file);
        tmp = tmp->next;
    }
}

void clear_input_buffer(FILE *file)
{
    int ch;
    while ((ch = fgetc(file)) != '\n' && ch != EOF)
        ;
}

Liver *get_liver_stdin(statements *state)
{
    statements stm = nulls;
    char *surname = NULL;
    char *name = NULL;
    char *father_name = NULL;
    char *birth_day = NULL;
    char male;
    long double amount_income;
    while (stm != correct)
    {
        printf("\nInput the surname of liver : \n");
        surname = read_line(&stm, stdin);
        if (stm != correct)
        {
            stm = allocate_error;
            return NULL;
        }
        if (stm == invalid_input || !is_latin_str(surname))
        {
            free(surname);
            printf("Error invalid input, re-enter please\n");
            continue;
        }
    }
    printf("Surname was entered\n");
    stm = nulls;
    while (stm != correct)
    {
        printf("\nEnter the name of liver :\n");
        name = read_line(&stm, stdin);
        if (stm == allocate_error)
        {
            *state = allocate_error;
            free(surname);
            return NULL;
        }
        if (stm == invalid_input || !is_latin_str(name))
        {
            free(name);
            printf("Error invalid input, re-enter please\n");
            continue;
        }
    }
    stm = nulls;
    printf("Name was entered!\n");
    while (stm != correct && stm != empty)
    {
        printf("\nInput the patronymic of liver :\n");
        father_name = read_line(&stm, stdin);
        if (stm == allocate_error)
        {
            *state = allocate_error;
            free(name);
            free(surname);
            return NULL;
        }
        if (stm == empty)
        {
            free(father_name);
            father_name = NULL;
            printf("No patronymic is setted\n");
            continue;
        }
        if (!is_latin_str(father_name))
        {
            free(father_name);
            printf("Error : invalid input please re-enter\n");
            stm = invalid_input;
            continue;
        }
    }
    stm = nulls;
    printf("Patronymic was entered\n");
    while (stm != correct)
    {
        printf("\nPlease enter birth date in format DD.MM.YYYY :\n");
        birth_day = read_line(&stm, stdin);
        if (stm == allocate_error)
        {
            *state = allocate_error;
            free(name);
            free(surname);
            free(father_name);
            return NULL;
        }
        if (stm == empty)
        {
            free(birth_day);
            printf("Error : something invalid\n");
            continue;
        }
        struct tm in;
        if (strptime(birth_day, "%d.%m.%Y", &in) == NULL || in.tm_year > 123 || in.tm_year < 10)
        {
            free(birth_day);
            printf("Invalid input\n");
            stm = invalid_input;
            continue;
        }
    }
    stm = nulls;
    printf("Bith date was setted\n");
    while (stm != correct)
    {
        printf("\nInput the gender of liver (M or W) :\n");
        char choice;
        fflush(stdin);
        if (scanf("%c", &choice) == EOF)
        {
            printf("Error : something invalid\n");
            stm = invalid_input;
            continue;
        }
        int ch;
        if ((ch = getchar()) != '\n' && ch != EOF)
        {
            clear_input_buffer(stdin);
            printf("Error : something invalid\n");
            stm = invalid_input;
            continue;
        }
        if (choice != 'M' && choice != 'W')
        {
            printf("Incorrect gender, we are in Russia!\n");
            stm = invalid_input;
            continue;
        }
        male = choice;
        stm = correct;
    }
    stm = nulls;
    printf("Gender was entered\n");
    while (stm != correct)
    {
        printf("\nInput the income of liver:\n");
        char *income = read_line(&stm, stdin);
        if (stm == allocate_error)
        {
            *state = allocate_error;
            free(name);
            free(surname);
            free(father_name);
            free(birth_day);
            return NULL;
        }
        if (stm != correct || strlen(income) > 20)
        {
            free(income);
            stm = invalid_input;
            printf("Error : invalid input please re-enter\n");
            continue;
        }
        char *end;
        long double average_income = strtold(income, &end);
        if (*end != '\0' && *end != '\r' && *end != '\n' || end == income ||
            (average_income == 0.0 && errno == ERANGE) ||
            average_income >= HUGE_VAL - 10 || average_income <= -HUGE_VAL + 10 || average_income < 0 || income[0] == '-')
        {
            printf("Error : something invalid\n");
            free(income);
            stm = invalid_input;
            continue;
        }
        free(income);
        amount_income = average_income;
    }
    printf("All entered sucessfully\n");
    Liver *current_liver = (Liver *)malloc(sizeof(Liver));
    if (current_liver == NULL)
    {
        free(surname);
        free(name);
        free(father_name);
        free(birth_day);
        *state = allocate_error;
        return NULL;
    }
    current_liver->surname = surname;
    current_liver->name = name;
    current_liver->patronymic = father_name;
    current_liver->birth_day = birth_day;
    current_liver->gender = male;
    current_liver->average_income = amount_income;
    printf("Created sucessfully\n\n");
    return current_liver;
}
