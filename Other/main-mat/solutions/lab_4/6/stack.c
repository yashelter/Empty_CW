#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

typedef enum statements
{
    correct,
    allocate_error,
    invalid_input,
    invalid_file,
    runtime_error,
    invalid_brackets,
    invalid_arguments,
    invalid_operator
} statements;

typedef struct Stack_node
{
    char data;
    struct Stack_node *next;
} Stack_node;

typedef struct
{
    Stack_node *top;
} Stack;

void create_stack(Stack *s)
{
    s->top = NULL;
}

int is_empty_stack(Stack *s)
{
    return s->top == NULL;
}

statements push_back_stack(Stack *s, char item)
{
    Stack_node *new_stack_node = (Stack_node *)malloc(sizeof(Stack_node));
    if (new_stack_node == NULL)
    {
        return allocate_error;
    }

    new_stack_node->data = item;
    new_stack_node->next = s->top;
    s->top = new_stack_node;
    return correct;
}

char pop_stack(Stack *s)
{
    if (!is_empty_stack(s))
    {
        Stack_node *temp = s->top;
        char data = temp->data;
        s->top = temp->next;
        free(temp);
        return data;
    }
    return -1;
}

void clear_stack(Stack *s)
{
    while (!is_empty_stack(s))
    {
        pop_stack(s);
    }
}

typedef struct Node
{
    char data;
    struct Node *left;
    struct Node *right;
} Node;

Node *create_node(char data)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        return NULL;
    }

    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}

void free_node(Node *node)
{
    node->left = node->right = NULL;
    free(node);
    node = NULL;
}

void free_tree(Node *root)
{
    if (root == NULL)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free_node(root);
}

typedef struct Node_to_stack
{
    Node *data;
    struct Node_to_stack *next;
} Node_to_stack;

typedef struct
{
    Node_to_stack *top;
} Node_stack;

void node_stack_init(Node_stack *s)
{
    s->top = NULL;
}

int is_node_stack_empty(Node_stack *s)
{
    return s->top == NULL;
}

statements push_node(Node_stack *s, Node *item)
{
    Node_to_stack *new_stack_node = (Node_to_stack *)malloc(sizeof(Node_to_stack));
    if (new_stack_node == NULL)
    {
        return allocate_error;
    }

    new_stack_node->data = item;
    new_stack_node->next = s->top;
    s->top = new_stack_node;
    return correct;
}

Node *pop_node(Node_stack *s)
{
    if (!is_node_stack_empty(s))
    {
        Node_to_stack *temp = s->top;
        Node *data = temp->data;
        s->top = temp->next;
        free(temp);
        return data;
    }
    return NULL;
}

void clear_node_stack(Node_stack *s)
{
    while (!is_node_stack_empty(s))
    {
        pop_node(s);
    }
}