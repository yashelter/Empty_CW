#include "MemCell.c"

bool ending_symb(char s)
{
    return (s == '=' || s == ';' || s == '\0' || s == '+' || s == '-' || s == '%' || s == '/' || s == ' ' || s == '*');
}

char *scan_lexema(FILE *file, char *ending)
{
    char *string;
    int buf = 128;
    int lent = 0;

    string = (char *)malloc(sizeof(char) * buf);

    if (string == NULL)
    {
        return NULL;
    }
    char symb;

    while ((symb = getc(file)) != EOF && !ending_symb(symb))
    {
        if (symb == '\n' || symb == ' ')
        {
            continue;
        }
        if (lent + 1 > buf)
        {
            buf *= 2;
            char *temp = (char *)realloc(string, sizeof(char) * buf);
            if (temp == NULL)
            {
                free(string);
                return NULL;
            }
            string = temp;
        }
        string[lent++] = symb;
    }
    *ending = symb;

    if (lent == 0)
    {
        free(string);
        return NULL;
    }
    if (lent != buf)
    {
        char *temp = (char *)realloc(string, sizeof(char) * (lent + 1));
        if (temp == NULL)
        {
            free(string);
            return NULL;
        }
        string = temp;
    }
    string[lent] = '\0';
    return string;
}

statements run(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        return invalid_file;
    }

    char *in;
    char ending;
    Cells cells;
    cells.buf = 1;
    cells.len = 0;
    cells.cells = (MemoryCell **)malloc(sizeof(MemoryCell *));
    
    if (cells.cells == NULL)
    {
        fclose(file);
        return allocate_error;
    }

    while ((in = scan_lexema(file, &ending)) != NULL)
    {
        //printf("%s\n", in);
        if (ending == '=')
        {
            if (!is_latin(in))
            {
                free(in);
                fclose(file);
                return invalid_input;
            }
            MemoryCell *cl;
            bool created = false;
            int position = bin_search(&cells, in);
            if (position == -1)
            {
                cl = create_cell(in, 0);
                created = !created;
            }
            else
            {
                cl = cells.cells[position];
            }

            free(in);
            in = NULL;
            char new_ending;
            do
            {
                in = scan_lexema(file, &new_ending);

                int delta = 0;
                if (is_latin(in))
                {
                    int position = bin_search(&cells, in);
                    if (position == -1)
                    {
                        free(in);
                        fclose(file);
                        return invalid_input;
                    }
                    delta = cells.cells[position]->val;
                }
                else if (is_number(in))
                {
                    delta = atoi(in);
                }
                else
                {
                    free(in);
                    fclose(file);
                    return invalid_input;
                }

                if (ending == '+')
                {
                    cl->val += delta;
                }
                else if (ending == '*')
                {
                    cl->val *= delta;
                }
                else if (ending == '-')
                {
                    cl->val -= delta;
                }
                else if (ending == '%')
                {
                    cl->val %= delta;
                }
                else if (ending == '/')
                {
                    cl->val /= delta;
                }
                else if (ending == '=')
                {
                    cl->val = delta;
                }
                ending = new_ending;
                free(in);
                in = NULL;
            } while (ending != ';');

            //print_memory_cell(cl);
            if (created)
            {
                add_cell(&cells, cl);
            }
        }
        else if (strcmp(in, "print") == 0)
        {
            free(in);
            in = NULL;
            if (ending == ' ')
            {
                in = scan_lexema(file, &ending);
                if (ending != ';' || !is_latin(in))
                {
                    free(in);
                    in = NULL;
                    delete_cells(&cells);
                    fclose(file);
                    return invalid_input;
                }
                int position = bin_search(&cells, in);

                if (position == -1)
                {
                    free(in);
                    in = NULL;
                    fclose(file);
                    delete_cells(&cells);
                    return invalid_input;
                }
                
                print_memory_cell(cells.cells[position]);
            }
            else
            {
                print_vals(&cells);
            }
        }

        if (in != NULL)
        {
            free(in);
        }
    }
    // printf("%s %c\n", in, ending);

    // print_vals(&cells);
    fclose(file);
    delete_cells(&cells);
    return correct;
}