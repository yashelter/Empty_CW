#include "Node.c"


String *get_line(FILE *file, char *delimeters, statements *state)
{
    String *s;
    statements stm = create_string("", &s);
    if (stm != correct)
    {
        *state = stm;
        return NULL;
    }
    char symb;

    while (((symb = fgetc(file)) != EOF) && (strchr(delimeters, symb) == NULL))
    {
        stm = string_add_char(symb, s);
        // printf("%c, ", symb);
        if (stm != correct)
        {
            *state = stm;
            return NULL;
        }
    }
    *state = correct;
    if (symb == EOF)
    {
        *state = end_of;
    }
    return s;
}

bool load_nodes(Node **result)
{
    FILE *file = fopen("node_tree.txt", "r");
    if (file == NULL)
    {
        return false;
    }
    statements stm;
    Node *root = create_node("", &stm);

    if (stm != correct)
    {
        fclose(file);
        return false;
    }
    root->count = -1;

    char *separators = ">";
    statements state = correct;

    while (state != end_of)
    {
        String *s = get_line(file, separators, &state);
        if (state == end_of)
        {
            delete_string(s);
            break;
        }
        if (stm != correct && stm != end_of)
        {
            delete_node(&root);
            fclose(file);
            return false;
        }

        Node *addable = create_node_s(s, &stm);
        if (stm != correct)
        {
            delete_node(&root);
            delete_string(s);
            fclose(file);
            return false;
        }
        int val = 0;
        fscanf(file, "%d>\n", &val);
        addable->count = val;
        stm = add_node(root, addable);
        if (stm != correct)
        {
            delete_node(&root);
            delete_string(s);
            fclose(file);
            return false;
        }
    }
    fclose(file);
    *result = root;
    return true;
}
statements validate(int argc, char *argv[], char **sep)
{
    if (argc < 3)
    {
        return invalid_input;
    }
    char *answ = (char *)malloc(sizeof(char) * (argc - 1));
    answ[argc - 2] = '\0';
    if (answ == NULL)
    {
        return allocate_error;
    }
    for (int i = 2; i < argc; i++)
    {
        if (strlen(argv[i]) != 1)
        {
            free(answ);
            return invalid_input;
        }
        answ[i - 2] = argv[i][0];
    }
    *sep = answ;
    return correct;
}

bool validate_file(FILE *file)
{
    char symb = '-';
    while (((symb = fgetc(file)) != EOF) && symb != '>')
    {
    }
    return symb != '>';
}

statements build_tree(int argc, char *argv[], Node **resultings)
{
    char *separators = NULL;
    statements stm = validate(argc, argv, &separators);
    if (stm != correct)
    {
        return stm;
    }
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        free(separators);
        return invalid_file;
    }
    if (!validate_file(file))
    {
        fclose(file);
        free(separators);
        return invalid_input;
    }
    fclose(file);
    file = fopen(argv[1], "r");
    if (file == NULL)
    {
        free(separators);
        return invalid_file;
    }

    Node *root = create_node("", &stm);
    if (stm != correct)
    {

        free(separators);
        return invalid_file;
    }
    statements state = correct;
    while (state != end_of)
    {
        String *s = get_line(file, separators, &state);
        if (stm != correct && stm != end_of)
        {
            fclose(file);
            free(separators);
            delete_node(&root);
            return stm;
        }

        Node *addable = create_node_s(s, &stm);
        if (stm != correct)
        {
            fclose(file);
            free(separators);
            delete_node(&root);
            return stm;
        }
        stm = add_node(root, addable);
        if (stm != correct)
        {
            fclose(file);
            free(separators);
            delete_node(&root);
            return stm;
        }
    }
    free(separators);
    fclose(file);
    *resultings = root;

    print_nodes(root, 0);
    printf("Success\n");
    return correct;
    String *s;
    // create_string("work", &s);

    // Node *search;

    // find_node(root, s, &search);
    // print_node(search);

    backup_nodes(root);
    delete_node(&root);

    Node *new_root;

    load_nodes(&new_root);
    print_nodes(new_root, 0);

    delete_node(&new_root);
    printf("Success\n");
}
statements get_int(int *result)
{
    fflush(stdout);
    int x;
    char c;
    if (scanf("%d", &x) != 1)
    {
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
        return invalid_input;
    }
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
    if (x < 0)
    {
        return invalid_input;
    }
    *result = x;
    return correct;
}

void run(int argc, char *argv[])
{
    Node *root;
    statements stm = build_tree(argc, argv, &root);
    if (stm != correct)
    {
        return;
    }
    printf("\tAll avaliable commands :\n");
    printf(">>> <p> Trace tree\n");
    printf(">>> <w> Track one word\n");
    printf(">>> <n> Find the most used fords\n");
    printf(">>> <b> Back-up tree in file\n");
    printf(">>> <d> Back-up tree in file\n");
    printf(">>> <l> Load tree from file\n");
    printf(">>> <s> Search the longest and shortest\n");
    printf(">>> <q> exit from programm\n\n");

    while (true)
    {
        printf("\n\nChose the action >>> -:\n");

        char c, input;

        printf("-: ");
        int scanned = scanf("%c", &input);
        skip_empty(stdin, '\n', &c);
        fflush(stdout);
        fflush(stdin);

        if (scanned != 1)
        {
            printf("Error : invalid input\n");
            skip_empty(stdin, '\n', &c);
            continue;
        }

        if (input == 'p')
        {
            print_nodes(root, 0);
            printf("\n");
        }
        else if (input == 'w')
        {
            Node *needed;
            printf("Enter the word for search : ");
            String *s = get_line(stdin, "\n", &stm);
            if (stm != correct && stm != end_of)
            {
                delete_string(s);
                continue;
            }
            stm = find_node(root, s, &needed);
            delete_string(s);
            if (stm == correct){
            printf("Word was in text <%d> times\n", needed->count);}
            else{
               printf("Word was not found\n");
            }
        }
        else if (input == 'n')
        {
            printf("\nEnter the needed N : ");
            fflush(stdout);
            int N = 0, size = 0;

            stm = get_int(&N);

            if (stm != correct)
            {
                printf("Error : invalid input\n");
                skip_empty(stdin, '\n', &c);
                continue;
            }

            Node **nds = get_sorted(root, &size, &stm);
            // printf("%d, %d\n", N, size);
            if (stm != correct)
            {
                printf("Error : occured error (code %d)\n", stm);
                continue;
            }
            printf("Result is : \n");
            for (int i = 0; i < N && i < size; i++)
            {
                print_node(nds[i]);
            }
            free(nds);
            printf("End of result\n");
        }
        else if (input == 'b')
        {
            bool backuped = backup_nodes(root);
            if (!backuped)
            {
                printf("Error : can't back-up tree\n");
                continue;
            }
            printf("Succesfully back-up'ed\n");
        }
        else if (input == 'l')
        {
            Node *new_root;
            bool loaded = load_nodes(&new_root);
            if (!loaded)
            {
                printf("Error : can't load tree\n");
                continue;
            }
            delete_node(&root);
            root = new_root;
            printf("Readed Tree: \n\n");
            print_nodes(new_root, 0);
        }
        else if (input == 's')
        {
            int size = 0;
            Node **nds = get_sorted(root, &size, &stm);
            // printf("%d, %d\n", N, size);
            if (stm != correct)
            {
                printf("Error : occured error (code %d)\n", stm);
                continue;
            }
            if (size > 1)
            {
                String *shortest = (nds[1])->data;
                String *longest = (nds[1])->data;

                for (int i = 2; i < size; i++)
                {
                    if (shortest->length > ((nds[i])->data->length))
                    {
                        shortest = nds[i]->data;
                    }
                    if (longest->length < ((nds[i])->data->length))
                    {
                        longest = nds[i]->data;
                    }
                }
                printf("The longest lexema is : %s\n", longest->value);
                printf("The shortest lexema is : %s\n", shortest->value);
            }
            free(nds);
        }
        else if (input == 'd')
        {
            int deep = deep_node(root) - 1;
            printf("The deep is : %d\n", deep);
        }
        else if (input == 'q')
        {
            delete_node(&root);
            printf("Work ended\n");
            return;
        }
        else{
            printf("Error : invalid input\n");
        }
    }
}