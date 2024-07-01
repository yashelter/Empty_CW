#include "MultiNode.c"

statements process_line(String *bline, Node **result)
{
    char *line = bline->value;
    int size = bline->length;
    Node *root;

    String *s;
    statements stm = create_string("root", &s);

    if (stm != correct)
    {
        return stm;
    }
    stm = create_node(s, &root);

    if (stm != correct)
    {
        delete_string(s);
        return stm;
    }
    char symbol;
    int i = 0;

    s = NULL;

    stm = create_string("", &s);
    if (stm != correct)
    {
        delete_nodes(root);
        return stm;
    }

    while (i < size)
    {
        symbol = line[i];
        Node *nd = NULL;
        // printf("<%c> - - - <%s>\n", symbol, root->data->value);
        if (symbol == ' ')
        {
            i++;
            continue;
        }
        if (s->length > 0 && (symbol == '(' || symbol == ',' || symbol == ')' || i == size - 1))
        {
            // print_string(s);

            stm = create_node(s, &nd);
            if (stm != correct)
            {
                delete_nodes(root);
                delete_string(s);
                return stm;
            }

            add_child(root, nd);
            // printf("\n");
            stm = create_string("", &s);
            if (stm != correct)
            {
                delete_nodes(root);
                return stm;
            }
        }
        else if (symbol != '(' && symbol != ')' && symbol != ',')
        {
            String *temp;
            char *lk = (char *)malloc(sizeof(char) * 2);
            if (lk == NULL)
            {
                delete_nodes(root);
                delete_string(s);
                return allocate_error;
            }
            sprintf(lk, "%c", symbol);

            stm = create_string(lk, &temp);
            free(lk);
            if (stm != correct)
            {
                delete_nodes(root);
                delete_string(s);
                return stm;
            }
            stm = _s_concat_string(s, temp);
            // print_string(s);
            if (stm != correct)
            {
                delete_string(temp);
                delete_nodes(root);
                delete_string(s);
                return stm;
            }
        }

        if (symbol == '(')
        {
            root = nd == NULL ? root : nd;
        }
        else if (symbol == ')')
        {
            root = root->master == NULL ? root : root->master;
        }
        i++;
    }
    *result = root;
    // print_nodes(root, 0);
    // delete_nodes(root);
    delete_string(s);
    return correct;
}

statements process_file(char *path, char *out_path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        return invalid_file;
    }
    FILE *out = fopen(out_path, "w");
    if (out == NULL)
    {
        fclose(file);
        return invalid_file;
    }

    String *line = NULL;
    statements stm = create_string("\0", &line);
    if (stm != correct)
    {
        fclose(file);
        fclose(out);
        return stm;
    }

    char ch;
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n' && line->length > 0 && line != NULL && line->value != NULL)
        {
            // printf("<%s>\n", line->value);
            stm = string_add_char(',', line);
            if (stm != correct)
            {
                return stm;
            }
            Node *result;
            stm = process_line(line, &result);
            delete_string(line);
            line = NULL;
            if (stm != correct)
            {
                fclose(file);
                fclose(out);
                return stm;
            }

            fprint_nodes(out, result, 0);

            delete_nodes(result);

            stm = create_string("", &line);
            if (stm != correct)
            {
                fclose(file);
                fclose(out);
                return stm;
            }
        }
        else if (ch != '\n' && ch != ' ')
        {
            stm = string_add_char(ch, line);
            if (stm != correct)
            {
                delete_string(line);
                fclose(file);
                fclose(out);
                return stm;
            }
        }
        // printf("%c", ch);
    }
    delete_string(line);
    fclose(file);
    fclose(out);
}

statements validate_input(int argc, char *argv[])
{
    if (argc != 3)
    {
        return invalid_input;
    }
    return correct;
}

void run(int argc, char *argv[])
{
    statements stm = validate_input(argc, argv);
    if (stm != correct){
        printf("Getted errror code %d\n", stm);
        return;
    }
    stm = process_file(argv[1], argv[2]);
    if (stm != correct){
        printf("Getted errror code %d\n", stm);
    }else{
        printf("Successfuly ended work\n");
    }
}