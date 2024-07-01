#include "stack.c"

#define MAX_LEN_FILENAME 10
#define MIN_LEN_FILENAME 5
#define FOR_TXT 5

// проверка что символ является оператором
bool is_operator(char data)
{
    char *operators = "&|~-+<=!?>";
    int i = 0;
    while (*operators != '\0')
    {
        if (*operators == data)
        {
            return true;
        }
        operators++;
    }
    return false;
}

// печатаем переменные
void print_var(FILE *file, const char *variables, int count)
{
    for (int i = 0; i < count; i++)
    {
        fprintf(file, " %c |", variables[i]);
    }
}

// печатаем выражение из дерева, в ификсной форме
void print_infix(FILE *file, Node *root)
{
    if (root == NULL)
        return;

    if (is_operator(root->data))
    {
        fprintf(file, "(");
    }

    print_infix(file, root->left);
    fprintf(file, "%c", root->data);

    if (root->data == '-' || root->data == '+' || root->data == '<')
    {
        fprintf(file, ">");
    }

    print_infix(file, root->right);

    if (is_operator(root->data))
    {
        fprintf(file, ")");
    }
}

// вывод заголовка таблицы
void print_header(FILE *file, const char *variables, int count, Node *root)
{
    print_var(file, variables, count);
    fprintf(file, " ");

    print_infix(file, root);
    fprintf(file, "\n");
}

// приоритет операций
int priority(char data)
{
    switch (data)
    {
    case '~':
        return 3;
    case '?':
    case '!':
    case '+':
    case '&':
        return 2;
    case '|':
    case '-':
    case '<':
    case '=':
        return 1;
    default:
        return -1;
    }
}

// двухсимвольный ли оператор
bool is_two_char_operator(char op1, char op2)
{
    char *two_ops[3] = {"->", "<>", "+>"};
    for (int i = 0; i < 3; i++)
    {
        if (op1 == two_ops[i][0] && op2 == two_ops[i][1])
        {
            return true;
        }
    }
    return false;
}

bool not_in_array(char *mas, int l, char symbol)
{
    for (int i = 0; i < l; i++)
    {
        if (mas[i] == symbol)
        {
            return false;
        }
    }
    return true;
}

// считаем уникальные переменные
int cnt_unique_lets(char **mas, char *infix)
{
    int length = 0;
    char *temp;
    int len = strlen(infix);
    for (int i = 0; i < len; i++)
    {
        if (isalpha(infix[i]) && not_in_array(*mas, length, infix[i]))
        {
            length++;
            temp = (char *)realloc(*mas, length * sizeof(char));
            if (temp == NULL)
            {
                if (*mas != NULL)
                {
                    free(*mas);
                }
                free(infix);
                return allocate_error;
            }
            *mas = temp;
            (*mas)[length - 1] = infix[i];
        }
    }
    return length;
}

// псевдослучайное имя
char *generate_filename()
{
    srand(time(NULL));

    int len = rand() % (MAX_LEN_FILENAME - MIN_LEN_FILENAME + 1) + MIN_LEN_FILENAME;

    char *file = (char *)malloc((len + FOR_TXT + 1) * sizeof(char));
    for (int i = 0; i <= len;)
    {
        file[i] = rand() % ('Z' - '0' + 1) + '0';
        if (isalpha(file[i]) || isdigit(file[i]))
        {
            i++;
        }
    }
    strcat(file, ".txt");
    return file;
}

// считаем выражение по дереву, со значяениями переменных values
int solution_from_tree(Node *root, const char *variables, const int *values, const int count)
{
    if (root == NULL)
        return -10;

    int left = -10, right = -10;

    if (isalpha(root->data))
    {
        for (int i = 0; i < count; i++)
        {
            if (root->data == variables[i])
            {
                return values[i];
            }
        }
    }
    else
    {
        left = solution_from_tree(root->left, variables, values, count);
        right = solution_from_tree(root->right, variables, values, count);
    }

    if (root->data == '0' || root->data == '1')
        return root->data - '0';

    if (root->data == '&')
    {
        return left & right;
    }
    if (root->data == '|')
    {
        return left | right;
    }
    if (root->data == '~')
    {
        if (left == -10)
        {
            left = right;
        }
        return ~left;
    }
    if (root->data == '-')
    {
        return left <= right;
    }
    if (root->data == '+')
    {
        return ~(~left | right);
    }
    if (root->data == '<')
    {
        return left != right;
    }
    if (root->data == '=')
    {
        return left == right;
    }
    if (root->data == '!')
    {
        return ~(left & right);
    }
    if (root->data == '?')
    {
        return ~(left | right);
    }

    return -10;
}

// построение таблицы истинности
statements build_table(Node *root, const int count, const char *mas, char **out_path)
{
    char *output_file = generate_filename();
    FILE *file = fopen(output_file, "w");
    if (file == NULL)
    {
        free(output_file);
        return invalid_file;
    }
    Node *current = root;

    print_header(file, mas, count, root);

    int num_combinations = 1 << count;
    int means[count], result;

    for (int i = 0; i < num_combinations; i++)
    {
        for (int j = 0; j < count; j++)
        {
            int bit = (i >> j) & 1;
            fprintf(file, " %d |", bit);
            means[j] = bit;
        }
        // результат при таких значениях переменных
        result = solution_from_tree(root, mas, means, count);

        if (result == -10)
        {
            fclose(file);
            free(output_file);
            return invalid_arguments;
        }
        fprintf(file, " %d\n", result);
    }
    fclose(file);
    *out_path = output_file;
    return correct;
}

// перевод инфиксной в постфиксную
statements infix_to_postfix(char **old_n, const char *new_n, int *leng)
{
    Stack stack;
    create_stack(&stack);

    int infix_len = strlen(new_n);
    char *resulting = (char *)malloc((2 * infix_len + 1) * sizeof(char));
    if (resulting == NULL)
    {
        return allocate_error;
    }

    int str_i = 0;
    int counter = 0;

    while (str_i < infix_len)
    {
        char symb = new_n[str_i];

        if (isspace(symb))
        {
            str_i++;
            continue;
        }

        if (isalnum(symb))
        {
            if (isdigit(symb) && symb != '0' && symb != '1')
            {
                free(resulting);
                clear_stack(&stack);
                return invalid_arguments;
            }
            resulting[counter++] = symb;
        }
        else if (symb == '(')
        {
            if (push_back_stack(&stack, symb) != correct)
            {
                free(resulting);
                clear_stack(&stack);
                return allocate_error;
            }
        }
        else if (symb == ')')
        {
            while (!is_empty_stack(&stack) && stack.top->data != '(')
            {
                resulting[counter++] = pop_stack(&stack);
            }
            pop_stack(&stack); // Удаляем '(' из стека
        }
        else
        {
            // Обработка двухсимвольных операторов
            if (!is_empty_stack(&stack) && priority(stack.top->data) >= priority(symb) &&
                stack.top->data != '(' && is_two_char_operator(stack.top->data, new_n[str_i + 1]))
            {
                resulting[counter++] = pop_stack(&stack);
            }
            if (push_back_stack(&stack, symb) != correct)
            {
                free(resulting);
                clear_stack(&stack);
                return allocate_error;
            }

            if (is_two_char_operator(symb, new_n[str_i + 1]))
            {
                str_i++;
            }
        }

        str_i++;
    }

    while (!is_empty_stack(&stack))
    {
        resulting[counter++] = pop_stack(&stack);
    }

    resulting[counter] = '\0';
    *old_n = resulting;
    *leng = counter;

    clear_stack(&stack);

    return correct;
}

statements build_tree(Node **tree, char *input)
{
    char *postfix = NULL;
    int length = 0;

    statements result = infix_to_postfix(&postfix, input, &length);
    if (result != correct)
        return result;

    Node_stack stack;
    node_stack_init(&stack);
    int index = 0;
    while (postfix[index] != '\0')
    {

        char symbol = postfix[index];
        if (isalnum(symbol) || symbol == '0' || symbol == '1')
        {
            Node *node = create_node(symbol);
            if (node == NULL || push_node(&stack, node) != correct)
            {
                clear_node_stack(&stack);
                free(postfix);
                return allocate_error;
            }
        }
        else
        {
            Node *new_node = NULL;
            if (is_two_char_operator(symbol, postfix[index + 1]))
            {
                char two_char_operator[3] = {symbol, postfix[index + 1], '\0'};
                index++;
                new_node = create_node(two_char_operator[0]);
            }
            else
                new_node = create_node(symbol);

            Node *right_node = pop_node(&stack);
            Node *left_node = pop_node(&stack);

            new_node->left = left_node;
            new_node->right = right_node;
            if (push_node(&stack, new_node) != correct)
            {
                clear_node_stack(&stack);
                free(postfix);
                return allocate_error;
            }
        }
        index++;
    }

    *tree = pop_node(&stack);
    clear_node_stack(&stack);
    free(postfix);

    return correct;
}

statements run(char *input_file, char **output_file)
{
    FILE *file = fopen(input_file, "r");
    if (file == NULL)
    {
        return invalid_file;
    }

    char *line = NULL;

    size_t bufsize = 0;
    size_t len = getline(&line, &bufsize, file);

    if (len < 1 || !feof(file))
    {
        free(line);
        fclose(file);
        return invalid_file;
    }
    fclose(file);

    Node *root = NULL;

    statements state = build_tree(&root, line);
    if (state != correct)
    {
        free(line);
        return state;
    }

    char *variables = NULL;
    int count = cnt_unique_lets(&variables, line);

    free(line);

    if (count < 0)
    {
        free_tree(root);
        return allocate_error;
    }

    state = build_table(root, count, variables, output_file);
    free(variables);
    free_tree(root);

    return state;
}

void exit_state(statements state)
{
    if (state == correct)
    {
        printf("Success\n");
    }
    else if (state == allocate_error)
    {
        printf("Error: Memory allocation failure\n");
    }
    else if (state == invalid_input)
    {
        printf("Error: Wrong input\n");
    }
    else if (state == invalid_file)
    {
        printf("Error: File error\n");
    }
    else if (state == runtime_error)
    {
        printf("Error: Runtime error\n");
    }
    else if (state == invalid_brackets)
    {
        printf("Error: Wrong number of brackets\n");
    }
    else if (state == invalid_arguments)
    {
        printf("Error: Wrong arguments\n");
    }
    else if (state == invalid_operator)
    {
        printf("Error: Wrong operator\n");
    }
    else
    {
        printf("Unknown state\n");
    }
}