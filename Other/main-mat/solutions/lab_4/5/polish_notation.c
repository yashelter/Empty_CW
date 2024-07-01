#include "stack.c"

bool ending_symb(char s)
{
    return (s == '+' || s == '-' || s == '%' || s == '/' || s == '^' || s == '*' || s == '(' || s == ')');
}

// с пропуском пробелов они не нужны
char *scan_line(FILE *file, int *resultin_cnt)
{
    char *string;
    int buf = 128;
    int lent = 0;
    char symb;

    string = (char *)malloc(sizeof(char) * buf);

    if (string == NULL)
    {
        return NULL;
    }

    while ((symb = getc(file)) != EOF && symb != '\n')
    {
        if (symb == ' ')
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
    *resultin_cnt = lent;
    return string;
}

bool is_number(const char *s)
{
    int l = strlen(s);
    for (int i = 0; i < l; i++)
    {
        if (!isdigit(s[i]))
        {
            return false;
        }
    }
    return true;
}

char **my_split(char *line, char *bad, int *cnts)
{
    if (cnts == NULL)
    {
        return NULL;
    }
    int buf = 128;
    int l = strlen(line), k = strlen(bad);

    char **mas = (char **)malloc(sizeof(char *) * buf);
    int curent_i = 0, counter = 0;

    if (mas == NULL)
    {
        return NULL;
    }

    while (counter < l)
    {
        bool was_bad = false;
        int mbuf = 128, cnt = 0;
        mas[curent_i] = (char *)malloc(sizeof(char) * mbuf);
        if (mas[curent_i] == NULL)
        {
            for (int i = 0; i < curent_i; i++)
            {
                free(mas[i]);
            }
            free(mas);
            return NULL;
        }

        for (; counter < l; counter++)
        {
            char symbol = line[counter];
            for (int j = 0; j < k; j++)
            {
                if (symbol == bad[j])
                {
                    was_bad = true;
                    break;
                }
            }
            if (was_bad)
            {
                break;
            }
            if (cnt + 1 >= mbuf)
            {
                mbuf *= 2;
                char *temp = (char *)realloc(mas[curent_i], sizeof(char) * mbuf);
                if (temp == NULL)
                {
                    for (int i = 0; i <= curent_i; i++)
                    {
                        free(mas[i]);
                    }
                    free(mas);
                    return NULL;
                }
                mas[curent_i] = temp;
            }
            mas[curent_i][cnt++] = symbol;
        }
        // printf("%suwu\n", mas[curent_i]);
        if (cnt == 0)
        {
            free(mas[curent_i]);
        }
        else
        {
            mas[curent_i][cnt] = '\0';
            char *temp = (char *)realloc(mas[curent_i], sizeof(char) * (cnt + 1));
            if (temp == NULL)
            {
                for (int i = 0; i <= curent_i; i++)
                {
                    free(mas[i]);
                }
                free(mas);
                return NULL;
            }
            mas[curent_i] = temp;
            curent_i++;
        }

        if (was_bad)
        {
            mas[curent_i] = (char *)malloc(sizeof(char) * 2);
            if (mas[curent_i] == NULL)
            {
                for (int i = 0; i < curent_i; i++)
                {
                    free(mas[i]);
                }
                free(mas);
                return NULL;
            }
            mas[curent_i][0] = line[counter];
            mas[curent_i][1] = '\0';
            curent_i++;
            counter++;
        }
    }
    *cnts = curent_i;
    return mas;
}

int eval(char **to, const char *lf, const char *rg, char op)
{
    int a = atoi(lf), b = atoi(rg);
    int result;
    if (op == '+')
    {
        result = snprintf(*to, sizeof(*to), "%d", a + b);
    }
    if (op == '-')
    {
        result = snprintf(*to, sizeof(*to), "%d", a - b);
    }
    if (op == '*')
    {
        result = snprintf(*to, sizeof(*to), "%d", a * b);
    }
    if (op == '/')
    {
        if (b == 0)
            return -1;
        result = snprintf(*to, sizeof(*to), "%d", a / b);
    }
    if (op == '%')
    {
        if (b == 0)
            return -1;
        result = snprintf(*to, sizeof(*to), "%d", a % b);
    }
    if (op == '^')
    {
        result = snprintf(*to, sizeof(*to), "%d", (int)pow(a, b));
    }
    return result;
}
void cleanup_memory(char ***ops, int l, Stack **a, char **st)
{
    for (int i = 0; i < l; i++)
    {
        free((*ops)[i]);
        (*ops)[i] = NULL;
    }
    free(*ops);
    if (st != NULL)
    {
        free(*st);
    }
    delete_stack(a);

    *ops = NULL;
}
statements solve(char *line, char **result)
{
    // if (line == NULL || result == NULL || answ == NULL)
    //{
    //     return runtime_error;
    // }
    int lent = strlen(line);
    Stack *stack = create_stack();
    if (stack == NULL)
    {
        return allocate_error;
    }

    char *operators = "+-*/^%()";
    char *lexema = NULL;
    char op;

    int l;

    char **ops = my_split(line, operators, &l);

    if (ops == NULL)
    {
        delete_stack(&stack);
        return allocate_error;
    }

    char *output = (char *)(malloc(sizeof(char) * (lent + l + 2))); // l - для пробелов
    char *out = output;

    if (output == NULL)
    {
        cleanup_memory(&ops, l, &stack, &output);
    }
    for (int i = 0; i < l; i++)
    {
        if (is_number(ops[i]))
        {
            out += sprintf(out, "%s ", ops[i]);
        }

        if (ops[i][0] == '+' || ops[i][0] == '-')
        {
            Node *nd = create_node(ops[i]);
            if (nd == NULL)
            {
                cleanup_memory(&ops, l, &stack, &output);
                return allocate_error;
            }

            while (stack->top != NULL && ((stack->top->symb)[0] != '+' && (stack->top->symb)[0] != '-' && (stack->top->symb)[0] != '('))
            {
                Node *cur = stack_pop(stack);
                out += sprintf(out, "%s ", cur->symb);
                delete_node(cur);
            }
            push_back(stack, nd);
        }
        if (ops[i][0] == '*' || ops[i][0] == '/' || ops[i][0] == '%' || ops[i][0] == '^')
        {
            Node *nd = create_node(ops[i]);
            if (nd == NULL)
            {
                cleanup_memory(&ops, l, &stack, &output);
                return allocate_error;
            }
            while (stack->top != NULL && ((stack->top->symb)[0] != '+' && (stack->top->symb)[0] != '-' && (stack->top->symb)[0] != '(' &&
                                          (stack->top->symb)[0] != '*' && (stack->top->symb)[0] != '/' && (stack->top->symb)[0] != '%'))
            {
                Node *cur = stack_pop(stack);
                out += sprintf(out, "%s ", cur->symb);
                delete_node(cur);
            }
            push_back(stack, nd);
        }
        if (ops[i][0] == '(')
        {
            Node *nd = create_node(ops[i]);
            if (nd == NULL)
            {
                cleanup_memory(&ops, l, &stack, &output);
                return allocate_error;
            }
            push_back(stack, nd);
        }
        if (ops[i][0] == ')')
        {
            bool flag = false;
            while (stack->top != NULL && !flag)
            {
                Node *cur = stack_pop(stack);
                if ((cur->symb)[0] == '(')
                {
                    delete_node(cur);
                    flag = true;
                    break;
                }
                out += sprintf(out, "%s ", cur->symb);
                delete_node(cur);
            }
            if (!flag)
            {
                cleanup_memory(&ops, l, &stack, &output);
                return invalid_notation;
            }
        }
    }
    Node *nd;
    while ((nd = stack_pop(stack)) != NULL)
    {
        out += sprintf(out, "%s ", nd->symb);
        if (nd->symb[0] == '(')
        {
            delete_node(nd);
            cleanup_memory(&ops, l, &stack, &output);
            return invalid_notation;
        }
        delete_node(nd);
    }
    cleanup_memory(&ops, l, &stack, NULL);

    *result = output;
    return correct;
}

void bfree(char **v, int l)
{
    if (v == NULL)
    {
        return;
    }
    for (int i = 0; i < l; i++)
    {
        free(v[i]);
        v[i] = NULL;
    }
    free(v);
}
statements calculate(char *rev_polish, int *asnw)
{
    int cnt = 0;
    char **vals = my_split(rev_polish, " ", &cnt);
    if (vals == NULL)
    {
        return allocate_error;
    }
    Stack *stack = create_stack();
    if (stack == NULL)
    {
        bfree(vals, cnt);
    }
    for (int i = 0; i < cnt; i++)
    {
        if (strcmp(vals[i], " ") == 0)
        {
            free(vals[i]);
            vals[i] = NULL;
            continue;
        }

        if (is_number(vals[i]))
        {
            Node *cur = create_node(vals[i]);
            if (cur == NULL)
            {
                bdelete_stack(&stack);
                bfree(vals + i, cnt);
                return allocate_error;
            }
            push_back(stack, cur);
            continue;
        }
        else
        {
            Node *right = stack_pop(stack), *left = stack_pop(stack);

            if (left == NULL || right == NULL)
            {
                bfree(vals, cnt);
                bdelete_stack(&stack);

                return invalid_notation;
            }
            char *buf = (char *)malloc(sizeof(char) * 20);
            if (buf == NULL)
            {
                bdelete_stack(&stack);
                bfree(vals + i, cnt);
                return allocate_error;
            }
            int res = eval(&buf, left->symb, right->symb, vals[i][0]);

            bdelete_node(left);
            bdelete_node(right);

            if (!(res > 0 && res < sizeof(buf)))
            {
                bdelete_stack(&stack);
                bfree(vals + i, cnt);
                return invalid_notation;
            }
            Node *nd = create_node(buf);
            nd->need_clean = true;
            if (nd == NULL)
            {
                bdelete_stack(&stack);
                bfree(vals + i, cnt);
                return allocate_error;
            }
            push_back(stack, nd);
        }
    }
    if (stack->top == NULL || stack->top->next != NULL)
    {
        bfree(vals, cnt);
        bdelete_stack(&stack);
        return invalid_notation;
    }

    *asnw = atoi(stack->top->symb);
    bfree(vals, cnt);
    bdelete_stack(&stack);
    return correct;
}