#include "functions.c"

void printp()
{
    printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
    printf("\nChoose action:\n");
    printf("-: a - create a new liver\n");
    printf("-: с - change data of certain liver\n");
    printf("-: d - delete liver\n");
    printf("-: p - trace livers to output file\n");
    printf("-: f - find the liver by your data\n");
    printf("-: u - ctrl+z\n");
    printf("-: i - info about all livers by sorted\n");
    printf("-: q - quit\n");
    printf("? -: ");
}

void printq()
{
    printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
    printf("What do you wish to change in this liver?\n");
    printf("-: s - surname\n");
    printf("-: n - name\n");
    printf("-: p - patronymic\n");
    printf("-: d - birth data\n");
    printf("-: g - gender\n");
    printf("-: a - average income\n");
    printf("-: q - cancel\n-?:\n");
}
statements run(list *lst, undo_stack *stack)
{
    int count_of_operation = 0;
    int can_undo = 0;
    while (1)
    {
        printp();
        char act;
        fflush(stdin);

        if (scanf("%c", &act) == EOF)
        {
            break;
        }
        int c;
        if ((c = getchar()) != '\n' && c != EOF)
        {
            clear_input_buffer(stdin);
            printf("Please re-enter : \n");
            continue;
        }

        if (act == 'a')
        {
            printf("Adding a new liver...\n");
            statements state = correct;
            Liver *searchable = get_liver_stdin(&state);
            if (searchable == NULL && state != correct)
            {
                return allocate_error;
            }
            node *tmp = find_same_liver(lst, searchable);
            if (tmp != NULL && lst->in_head != NULL)
            {
                delete_liver(searchable);
                printf("It is a copy of existing, can't add\n");
                continue;
            }
            state = add_node(lst, searchable);
            if (state == allocate_error)
            {
                delete_liver(searchable);
                return allocate_error;
            }
            if (state == invalid_input)
            {
                delete_liver(searchable);
                printf("Getted error in input\n");
                continue;
            }

            node *current_node = find_same_liver(lst, searchable);
            if (current_node == NULL)
            {
                return allocate_error;
            }

            Liver *stack_liver = copy_liver(searchable);
            if (stack_liver == NULL)
            {
                return allocate_error;
            }

            node *temp = create_node(stack_liver);
            if (temp == NULL)
            {
                delete_liver(stack_liver);
                return allocate_error;
            }

            Liver *prev = copy_liver(current_node->liver);
            if (prev == NULL)
            {
                delete_liver(temp->liver);
                free(temp);

                return allocate_error;
            }
            node *current_in_list = create_node(prev);
            if (current_node == NULL)
            {
                delete_liver(temp->liver);
                free(temp);
                delete_liver(prev);
                return allocate_error;
            }

            statements stt = undostack_push_back(stack, temp, add, current_in_list);
            if (stt != correct)
            {
                delete_liver(temp->liver);
                free(temp);
                delete_liver(current_in_list->liver);
                free(current_in_list);

                return allocate_error;
            }
            print_undo_stack(stack);
            printf("All correct, liver added\n");
            count_of_operation++;

            continue;
        }
        else if (act == 'c')
        {
            printf("Looking for a existing liver : \n");
            statements stat = correct;
            Liver *searchable = get_liver_stdin(&stat);
            if (searchable == NULL && stat == allocate_error)
            {
                return allocate_error;
            }
            node *tmp = find_same_liver(lst, searchable);
            if (tmp == NULL && lst->in_head != NULL)
            {
                delete_liver(searchable);
                printf("Liver not found\n");
                continue;
            }
            while (1)
            {
                printq();

                char choice;
                fflush(stdin);
                // scanf("%c", &act);
                if (scanf("%c", &choice) == EOF)
                {
                    break;
                }
                int ch;
                if ((ch = getchar()) != '\n' && ch != EOF)
                {
                    clear_input_buffer(stdin);
                    printf("Please re-enter\n");
                    continue;
                }
                if (choice == 'q')
                {
                    delete_liver(searchable);
                    printf("quit\n");
                    break;
                }
                else if (choice == 's')
                {
                    statements st = nulls;
                    char *surname = NULL;
                    while (st != correct)
                    {
                        printf("\nInput the new surname of liver:\n");
                        surname = read_line(&st, stdin);
                        if (st == allocate_error)
                        {
                            delete_liver(searchable);
                            return allocate_error;
                        }
                        if (st == empty)
                        {
                            free(surname);
                            printf("Surname isn't empty string!\n");
                            continue;
                        }
                        if (!is_latin_str(surname))
                        {
                            free(surname);
                            printf("Surname must be a latin string!\n");
                            st = invalid_input;
                            continue;
                        }
                    }

                    Liver *stack_liver = copy_liver(searchable);
                    if (stack_liver == NULL)
                    {
                        delete_liver(searchable);
                        free(surname);
                        return allocate_error;
                    }
                    delete_liver(searchable);
                    free(tmp->liver->surname);
                    tmp->liver->surname = surname;
                    node *tmp2 = create_node(stack_liver);
                    if (tmp2 == NULL)
                    {
                        delete_liver(stack_liver);
                        return allocate_error;
                    }

                    Liver *prev_liver = copy_liver(tmp->liver);
                    if (prev_liver == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        return allocate_error;
                    }
                    node *tmp3 = create_node(prev_liver);
                    if (tmp3 == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        free(prev_liver);
                        return allocate_error;
                    }
                    statements stt = undostack_push_back(stack, tmp2, modification, tmp3);
                    if (stt != correct)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        delete_liver(tmp3->liver);
                        free(tmp3);
                        return allocate_error;
                    }
                    printf("The surname was changed!\n");
                    count_of_operation++;
                    print_undo_stack(stack);
                    break;
                }
                else if (choice == 'n')
                {
                    statements st = nulls;
                    char *name = NULL;
                    while (st != correct)
                    {
                        printf("\nInput the new name of liver:\n");
                        name = read_line(&st, stdin);
                        if (st == allocate_error)
                        {
                            free(searchable);
                            return allocate_error;
                        }
                        if (st == empty)
                        {
                            free(name);
                            printf("Name isn't empty string!\n");
                            continue;
                        }
                        if (!is_latin_str(name))
                        {
                            free(name);
                            printf("Name must be a latin string!\n");
                            st = invalid_input;
                            continue;
                        }
                    }

                    Liver *stack_liver = copy_liver(searchable);
                    if (stack_liver == NULL)
                    {
                        free(searchable);
                        free(name);
                        return allocate_error;
                    }
                    delete_liver(searchable);
                    free(tmp->liver->name);
                    tmp->liver->name = name;

                    node *tmp2 = create_node(stack_liver);
                    if (tmp2 == NULL)
                    {
                        delete_liver(stack_liver);
                        return allocate_error;
                    }

                    Liver *prev_liver = copy_liver(tmp->liver);
                    if (prev_liver == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        return allocate_error;
                    }
                    node *tmp3 = create_node(prev_liver);
                    if (tmp3 == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        delete_liver(prev_liver);
                        return allocate_error;
                    }
                    statements stt = undostack_push_back(stack, tmp2, modification, tmp3);
                    if (stt != correct)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        delete_liver(tmp3->liver);
                        free(tmp3);
                        return allocate_error;
                    }
                    printf("Name was changed!\n");
                    count_of_operation++;
                    print_undo_stack(stack);
                    break;
                }
                else if (choice == 'p')
                {
                    statements st = nulls;
                    char *father_name = NULL;
                    while (st != correct && st != empty)
                    {
                        printf("\nInput the new patronymic of liver:\n");
                        father_name = read_line(&st, stdin);
                        if (st == allocate_error)
                        {
                            delete_liver(searchable);
                            return allocate_error;
                        }
                        if (st == empty)
                        {
                            free(father_name);
                            father_name = NULL;
                            printf("Patranomic setted empty!\n");
                            continue;
                        }
                        if (!is_latin_str(father_name))
                        {
                            free(father_name);
                            printf("Patronymic must be a latin string!\n");
                            st = invalid_input;
                            continue;
                        }
                    }
                    Liver *stack_liver = copy_liver(searchable);
                    if (stack_liver == NULL)
                    {
                        delete_liver(searchable);
                        free(father_name);
                        return allocate_error;
                    }
                    delete_liver(searchable);
                    free(tmp->liver->patronymic);
                    tmp->liver->patronymic = father_name;
                    node *tmp2 = create_node(stack_liver);
                    if (tmp2 == NULL)
                    {
                        delete_liver(stack_liver);
                        return allocate_error;
                    }

                    Liver *prev_liver = copy_liver(tmp->liver);
                    if (prev_liver == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        return allocate_error;
                    }
                    node *tmp3 = create_node(prev_liver);
                    if (tmp3 == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        delete_liver(prev_liver);
                        return allocate_error;
                    }
                    statements stt = undostack_push_back(stack, tmp2, modification, tmp3);
                    if (stt != correct)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        free(tmp3->liver);
                        free(tmp3);
                        return allocate_error;
                    }
                    printf("The patronymic was changed!\n");
                    count_of_operation++;
                    print_undo_stack(stack);
                    break;
                }
                else if (choice == 'd')
                {
                    statements st = nulls;
                    char *birth_day = NULL;
                    while (st != correct)
                    {
                        printf("\nInput the new birth day of liver in format dd.mm.yyyy :\n");
                        birth_day = read_line(&st, stdin);
                        if (st == allocate_error)
                        {
                            delete_liver(searchable);
                            return allocate_error;
                        }
                        if (st == empty)
                        {
                            free(birth_day);
                            printf("Birth day cannot be an empty string!\n");
                            continue;
                        }
                        struct tm in;
                        if (strptime(birth_day, "%d.%m.%Y", &in) == NULL || in.tm_year > 123 || in.tm_year < 10)
                        {
                            free(birth_day);
                            printf("incorrect data!\n");
                            st = invalid_input;
                            continue;
                        }
                    }
                    Liver *stack_liver = copy_liver(searchable);
                    if (stack_liver == NULL)
                    {
                        delete_liver(searchable);
                        free(birth_day);
                        return allocate_error;
                    }

                    node *tmp2 = create_node(stack_liver);
                    if (tmp2 == NULL)
                    {
                        delete_liver(stack_liver);
                        return allocate_error;
                    }

                    free(searchable->birth_day);
                    searchable->birth_day = birth_day;

                    delete_node_list(lst, tmp);

                    statements sttt = add_node(lst, searchable);
                    if (sttt != correct)
                    {
                        delete_liver(searchable);
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        return allocate_error;
                    }

                    Liver *prev_liver = copy_liver(searchable);
                    if (prev_liver == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        return allocate_error;
                    }
                    node *tmp3 = create_node(prev_liver);
                    if (tmp3 == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        delete_liver(prev_liver);
                        return allocate_error;
                    }
                    statements stt = undostack_push_back(stack, tmp2, modification, tmp3);
                    if (stt != correct)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        delete_liver(tmp3->liver);
                        free(tmp3);
                        return allocate_error;
                    }
                    printf("The date of birth was changed!\n");
                    count_of_operation++;
                    print_undo_stack(stack);
                    break;
                }
                else if (choice == 'g')
                {
                    Liver *stack_liver = copy_liver(searchable);
                    if (stack_liver == NULL)
                    {
                        delete_liver(searchable);
                        return allocate_error;
                    }
                    delete_liver(searchable);

                    if (tmp->liver->gender == 'M')
                    {
                        tmp->liver->gender = 'W';
                    }
                    else
                    {
                        tmp->liver->gender = 'M';
                    }

                    node *tmp2 = create_node(stack_liver);
                    if (tmp2 == NULL)
                    {
                        delete_liver(stack_liver);
                        return allocate_error;
                    }

                    Liver *prev_liver = copy_liver(tmp->liver);
                    if (prev_liver == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        return allocate_error;
                    }
                    node *tmp3 = create_node(prev_liver);
                    if (tmp3 == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        delete_liver(prev_liver);
                        return allocate_error;
                    }
                    statements stt = undostack_push_back(stack, tmp2, modification, tmp3);
                    if (stt != correct)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        delete_liver(tmp3->liver);
                        free(tmp3);
                        return allocate_error;
                    }
                    printf("The male was changed to reverse\n");
                    print_undo_stack(stack);
                    count_of_operation++;
                    break;
                }
                else if (choice == 'a')
                {
                    statements st = nulls;
                    long double amount_income = 0L;
                    while (st != correct)
                    {
                        printf("\nInput the new income of liver:\n");
                        char *income = read_line(&st, stdin);
                        if (st == allocate_error)
                        {
                            delete_liver(searchable);
                            return allocate_error;
                        }
                        if (st == empty)
                        {
                            free(income);
                            printf("Income cannot be an empty string!\n");
                            continue;
                        }
                        if (strlen(income) > 20)
                        {
                            printf("Too large/little number!\n");
                            free(income);
                            st = invalid_input;
                            continue;
                        }
                        char *end;
                        long double average_income = strtold(income, &end);
                        if (*end != '\0' && *end != '\r' && *end != '\n' || end == income || (average_income == 0.0 && errno == ERANGE) || average_income >= HUGE_VAL - 10 || average_income <= -HUGE_VAL + 10 || average_income < 0 || income[0] == '-')
                        {
                            printf("You've inputted invalid or negative income!\n");
                            free(income);
                            st = invalid_input;
                            continue;
                        }
                        free(income);
                        amount_income = average_income;
                    }

                    Liver *stack_liver = copy_liver(searchable);
                    if (stack_liver == NULL)
                    {
                        return allocate_error;
                    }
                    delete_liver(searchable);
                    tmp->liver->average_income = amount_income;
                    node *tmp2 = create_node(stack_liver);
                    if (tmp2 == NULL)
                    {
                        delete_liver(stack_liver);
                        return allocate_error;
                    }

                    Liver *prev_liver = copy_liver(tmp->liver);
                    if (prev_liver == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        return allocate_error;
                    }
                    node *tmp3 = create_node(prev_liver);
                    if (tmp3 == NULL)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        delete_liver(prev_liver);
                        return allocate_error;
                    }
                    statements stt = undostack_push_back(stack, tmp2, modification, tmp3);
                    if (stt != correct)
                    {
                        delete_liver(tmp2->liver);
                        free(tmp2);
                        delete_liver(tmp3->liver);
                        free(tmp3);
                        return allocate_error;
                    }
                    printf("The average income was changed!\n");
                    count_of_operation++;
                    print_undo_stack(stack);
                    break;
                }
                else
                {
                    printf("You've inputted wrong action, rerun\n");
                    delete_liver(searchable);
                    continue;
                }
            }

            continue;
        }
        else if (act == 'd')
        {
            if (lst->in_head == NULL)
            {
                printf("The list is empty!\n");
                continue;
            }
            printf("Process of deleting a liver has been started\n");
            statements stat = correct;
            Liver *searchable = get_liver_stdin(&stat);
            if (searchable == NULL && stat == allocate_error)
            {
                return allocate_error;
            }
            node *tmp = find_same_liver(lst, searchable);
            if (tmp == NULL && lst->in_head != NULL)
            {
                delete_liver(searchable);
                printf("There no such liver!\n");
                continue;
            }

            Liver *stack_liver = copy_liver(searchable);
            if (stack_liver == NULL)
            {
                return allocate_error;
            }
            delete_liver(searchable);

            node *tmp2 = create_node(stack_liver);
            if (tmp2 == NULL)
            {
                delete_liver(stack_liver);
                return allocate_error;
            }
            statements stt = undostack_push_back(stack, tmp2, delete, NULL); // так как удалил
            if (stt != correct)
            {
                delete_liver(tmp2->liver);
                free(tmp2);
                return allocate_error;
            }

            print_undo_stack(stack);

            delete_node_list(lst, tmp);
            printf("Your liver was deleted/\n");
            count_of_operation++;
            continue;
        }
        else if (act == 'p')
        {
            printf("Prtinting to file.\nInput the output path :");
            statements stat = nulls;
            char *filename = NULL;
            while (stat != correct)
            {
                filename = read_line(&stat, stdin);
                if (stat == allocate_error)
                {
                    return allocate_error;
                }
                if (stat == empty)
                {
                    free(filename);
                    printf("Filename isn't empty string!\n");
                    continue;
                }
            }
            FILE *file = fopen(filename, "w");
            if (file == NULL)
            {
                printf("Something wrong with opening the file: %s\n", filename);
                free(filename);
                stat = invalid_input;
                continue;
            }
            print_list(lst, file);
            fclose(file);
            free(filename);
            printf("Printed\n");
            continue;
        }
        else if (act == 'f')
        {
            printf("Searching liver :\n");
            statements stat = correct;
            Liver *searchable = get_liver_stdin(&stat);
            if (searchable == NULL && stat == allocate_error)
            {
                return allocate_error;
            }

            node *tmp = find_same_liver(lst, searchable);
            if (tmp == NULL)
            {
                printf("Error : liver not found\n");
            }
            else
            {
                printf("Founded : \n\n");
                print_liver(tmp->liver, stdout);
            }
            delete_liver(searchable);

            continue;
        }
        else if (act == 'u')
        {
            printf("u\n");
            printf("Amount elements in stack: %d\n", stack->size);
            printf("Count of modification:%d:\n", count_of_operation);

            if (count_of_operation < 2)
            {
                printf("too little operation were done!\n");
                continue;
            }
            if (can_undo > count_of_operation / 2)
            {
            }
            else
            {
                can_undo = count_of_operation / 2;
                can_undo = stack->size - can_undo;
            }

            if (stack->size != can_undo)
            {
                if (stack->nodes[stack->size - 1].tp == add)
                {
                    node *tmp = find_same_liver(lst, stack->nodes[stack->size - 1].current_in_list->liver);
                    if (tmp == NULL)
                    {
                        return allocate_error;
                    }
                    delete_node_list(lst, tmp);

                    delete_liver(stack->nodes[stack->size - 1].pnode->liver);
                    free(stack->nodes[stack->size - 1].pnode);
                    if (stack->nodes[stack->size - 1].current_in_list != NULL)
                    {
                        delete_liver(stack->nodes[stack->size - 1].current_in_list->liver);
                        free(stack->nodes[stack->size - 1].current_in_list);
                    }
                    stack->size--;
                    stack->buf = stack->size;
                    stack_node *temp = (stack_node *)realloc(stack->nodes, sizeof(stack_node) * (stack->buf));
                    if (temp == NULL)
                    {
                        return allocate_error;
                    }
                    else
                    {
                        stack->nodes = temp;
                    }
                    count_of_operation--;
                    printf("Undo complete\n");
                }
                else if (stack->nodes[stack->size - 1].tp == delete)
                {

                    Liver *new_liver = copy_liver(stack->nodes[stack->size - 1].pnode->liver);
                    if (new_liver == NULL)
                    {
                        return allocate_error;
                    }
                    statements st = add_node(lst, new_liver);
                    if (st != correct)
                    {
                        delete_liver(new_liver);
                        return allocate_error;
                    }

                    delete_liver(stack->nodes[stack->size - 1].pnode->liver);
                    free(stack->nodes[stack->size - 1].pnode);
                    if (stack->nodes[stack->size - 1].current_in_list != NULL)
                    {
                        delete_liver(stack->nodes[stack->size - 1].current_in_list->liver);
                        free(stack->nodes[stack->size - 1].current_in_list);
                    }
                    stack->size--;
                    stack->buf = stack->size;
                    stack_node *temp = (stack_node *)realloc(stack->nodes, sizeof(stack_node) * (stack->buf));
                    if (temp == NULL)
                    {
                        return allocate_error;
                    }
                    else
                    {
                        stack->nodes = temp;
                    }
                    count_of_operation--;
                    printf("Undo complete\n");
                }

                else if (stack->nodes[stack->size - 1].tp == modification)
                {
                    node *tmp = find_same_liver(lst, stack->nodes[stack->size - 1].current_in_list->liver);
                    if (tmp == NULL)
                    {
                        return allocate_error;
                    }
                    delete_node_list(lst, tmp);
                    Liver *new_liver = copy_liver(stack->nodes[stack->size - 1].pnode->liver);
                    if (new_liver == NULL)
                    {
                        return allocate_error;
                    }
                    statements st = add_node(lst, new_liver);
                    if (st != correct)
                    {
                        delete_liver(new_liver);
                        return allocate_error;
                    }

                    delete_liver(stack->nodes[stack->size - 1].pnode->liver);
                    free(stack->nodes[stack->size - 1].pnode);
                    if (stack->nodes[stack->size - 1].current_in_list != NULL)
                    {
                        delete_liver(stack->nodes[stack->size - 1].current_in_list->liver);
                        free(stack->nodes[stack->size - 1].current_in_list);
                    }
                    stack->size--;
                    stack->buf = stack->size;
                    stack_node *temp = (stack_node *)realloc(stack->nodes, sizeof(stack_node) * (stack->buf));
                    if (temp == NULL)
                    {
                        return allocate_error;
                    }
                    else
                    {
                        stack->nodes = temp;
                    }
                    count_of_operation--;
                    printf("Undo complete\n");
                }
                else
                {
                }
            }
            // print_undo_stack(stack);

            continue;
        }
        else if (act == 'i')
        {
            print_list(lst, stdout);
            continue;
        }
        else if (act == 'q')
        {
            printf("Closing...\n");
            break;
        }
        else
        {
            printf("Error : invalid input\n");
            continue;
        }
    }
    return correct;
}
