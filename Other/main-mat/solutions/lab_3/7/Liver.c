//#define _XOPEN_SOURCE
//#include <time.h>
#include <errno.h>
#include <math.h>
#include <signal.h>

#include "../MonoBehavior.c"

#define EPS 1e-6

typedef struct
{
    char *surname;
    char *name;
    char *patronymic;
    char *birth_day;
    char gender;
    long double average_income;
} Liver;

typedef struct node
{
    Liver *liver;
    struct node *next;
} node;

typedef struct
{
    node *in_head;
} list;

typedef enum
{
    delete,
    add,
    modification
} type;

typedef struct
{
    type tp;
    node *pnode;
    node *current_in_list;
} stack_node;

typedef struct
{
    stack_node *nodes;
    int size;
    int buf;
} undo_stack;

void print_liver(Liver *liver, FILE *file)
{

    fprintf(file, "Surname is : %s\n", liver->surname);
    fprintf(file, "Name is : %s\n", liver->name);
    fprintf(file, "father_name: %s\n", liver->patronymic == NULL ? "no" : liver->patronymic);
    fprintf(file, "birthday: %s\n", liver->birth_day);
    fprintf(file, "gender: %c\n", liver->gender);
    fprintf(file, "average income: %Lf\n\n", liver->average_income);
}

int count_tokens(char *str, const char *params)
{
    int count = 0;
    char *token = strtok(str, params);
    while (token != NULL)
    {
        count++;
        token = strtok(NULL, params);
    }
    return count;
}

bool is_latin_str(char *str)
{
    if (str == NULL)
    {
        return false;
    }
    int len = (int)strlen(str);
    for (int i = 0; i < len; ++i)
    {
        if (!isalpha(str[i]))
        {
            return false;
        }
    }
    return true;
}


node *create_node(Liver *liver)
{
    if (liver == NULL)
    {
        return NULL;
    }
    node *new_node = (node *)malloc(sizeof(node));
    if (new_node == NULL)
    {
        return NULL;
    }
    new_node->liver = liver;
    new_node->next = NULL;
    return new_node;
}


statements add_node(list *lst, Liver *liver)
{
    if (lst == NULL || liver == NULL)
    {
        return invalid_input;
    }
    node *new_node = create_node(liver);
    if (new_node == NULL)
    {
        return allocate_error;
    }

    if (lst->in_head == NULL)
    {
        lst->in_head = new_node;
        return correct;
    }

    if (compare_date(lst->in_head->liver->birth_day, new_node->liver->birth_day) < 0)
    {
        new_node->next = lst->in_head;
        lst->in_head = new_node;
        return correct;
    }

    node *tmp = lst->in_head;
    while (tmp->next != NULL && compare_date(tmp->next->liver->birth_day, liver->birth_day) > 0)
    {
        tmp = tmp->next;
    }
    new_node->next = tmp->next;
    tmp->next = new_node;
    return correct;
}


node *find_same_liver(list *lst, Liver *liver)
{
    if (lst->in_head == NULL)
    {
        return NULL;
    }
    node *tmp = lst->in_head;
    while (tmp != NULL)
    {
        int surname = strcmp(tmp->liver->surname, liver->surname);
        int name = strcmp(tmp->liver->name, liver->name);
        int father_name;
        if (liver->patronymic != NULL && tmp->liver->patronymic != NULL)
        {
            father_name = strcmp(tmp->liver->patronymic, liver->patronymic);
        }
        else
        {
            father_name = 1;
        }

        int birth = strcmp(tmp->liver->birth_day, liver->birth_day);
        if (!surname && !name && ((!father_name) || (liver->patronymic == NULL && tmp->liver->patronymic == NULL)) && !birth && tmp->liver->gender == liver->gender && fabsl(tmp->liver->average_income - liver->average_income) < EPS)
        {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}


void delete_liver(Liver *liver)
{
    if (liver == NULL)
    {
        return;
    }
    free(liver->surname);
    free(liver->name);
    free(liver->patronymic);
    free(liver->birth_day);
    free(liver);
}


void free_list(list *lst)
{
    node *tmp = NULL;
    while (lst->in_head != NULL)
    {
        tmp = lst->in_head;
        lst->in_head = lst->in_head->next;
        delete_liver(tmp->liver);
        free(tmp);
    }
}


statements undostack_push_back(undo_stack *stack, node *prev, type tp, node *current_in_list)
{
    if (stack == NULL || prev == NULL)
    {
        return runtime_error;
    }

    if (stack->nodes == NULL)
    {
        stack->buf = 2;
        stack->size = 0;
        stack->nodes = (stack_node *)malloc(sizeof(stack_node) * (stack->buf));
        if (stack->nodes == NULL)
        {
            return allocate_error;
        }
    }

    stack->size++;
    if (stack->size >= stack->buf)
    {
        stack->buf *= 2;
        stack_node *tmp = (stack_node *)realloc(stack->nodes, sizeof(stack_node) * (stack->buf));
        if (tmp == NULL)
        {
            return allocate_error;
        }
        stack->nodes = tmp;
    }

    stack->nodes[stack->size - 1].pnode = prev;
    stack->nodes[stack->size - 1].tp = tp;
    stack->nodes[stack->size - 1].current_in_list = current_in_list;
    return correct;
}

void delete_stack(undo_stack *stack)
{
    if (stack == NULL)
    {
        return;
    }
    for (int i = 0; i < stack->size; ++i)
    {
        delete_liver(stack->nodes[i].pnode->liver);
        free(stack->nodes[i].pnode);
        if (stack->nodes[i].current_in_list != NULL)
        {
            delete_liver(stack->nodes[i].current_in_list->liver);
            free(stack->nodes[i].current_in_list);
        }
    }
    free(stack->nodes);
}

Liver *copy_liver(Liver *searchable)
{
    if (searchable == NULL)
    {
        return NULL;
    }
    Liver *stack_liver = (Liver *)malloc(sizeof(Liver));
    if (stack_liver == NULL)
    {
        return NULL;
    }
    stack_liver->surname = (char *)malloc(sizeof(char) * (strlen(searchable->surname) + 1));
    if (stack_liver->surname == NULL)
    {
        free(stack_liver);
        return NULL;
    }
    strcpy(stack_liver->surname, searchable->surname);
    stack_liver->name = (char *)malloc(sizeof(char) * (strlen(searchable->name) + 1));
    if (stack_liver->name == NULL)
    {
        free(stack_liver->surname);
        free(stack_liver);
        return NULL;
    }
    strcpy(stack_liver->name, searchable->name);
    stack_liver->patronymic = NULL;
    if (searchable->patronymic != NULL)
    {
        stack_liver->patronymic = (char *)malloc(sizeof(char) * (strlen(searchable->patronymic) + 1));
        if (stack_liver->patronymic == NULL)
        {
            free(stack_liver->surname);
            free(stack_liver->name);
            free(stack_liver);
            return NULL;
        }
        strcpy(stack_liver->patronymic, searchable->patronymic);
    }
    stack_liver->birth_day = (char *)malloc(sizeof(char) * (strlen(searchable->birth_day) + 1));
    if (stack_liver->birth_day == NULL)
    {
        free(stack_liver->surname);
        free(stack_liver->name);
        free(stack_liver->patronymic);
        free(stack_liver);
        return NULL;
    }
    strcpy(stack_liver->birth_day, searchable->birth_day);
    stack_liver->gender = searchable->gender;
    stack_liver->average_income = searchable->average_income;

    return stack_liver;
}

void print_undo_stack(undo_stack *stack)
{
    if (stack == NULL)
    {
        return;
    }

    for (int i = 0; i < stack->size; i++)
    {
        printf("Undo Stack Node %d:\n", i + 1);
        printf("Type: %d\n", stack->nodes[i].tp);
        print_liver(stack->nodes[i].pnode->liver, stdout);
        printf("current node in list:\n");
        if (stack->nodes[i].current_in_list == NULL)
        {
            printf("there no current liver as was deleted\n");
        }
        else
        {
            print_liver(stack->nodes[i].current_in_list->liver, stdout);
        }

        printf("\n");
    }
}

void delete_node_list(list *lst, node *tmp)
{
    if (lst->in_head == NULL)
    {
        return;
    }
    node *temp = lst->in_head;
    if (temp == tmp)
    {
        lst->in_head = lst->in_head->next;
        delete_liver(tmp->liver);
        free(tmp);
        return;
        // printf("your liver was deleted/\n");
    }

    while (temp->next != tmp)
    {
        temp = temp->next;
    }
    temp->next = tmp->next;
    delete_liver(tmp->liver);
    free(tmp);
}