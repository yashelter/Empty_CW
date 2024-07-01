#include "../4/String.c"

#define NODE_CHILDREN 8

typedef struct Node Node;

struct Node
{
    Node *left_child;
    Node *right_child;

    Node *master;
    String *data;
    int count;
};

Node *create_node(char *value, statements *st)
{
    String *s;
    *st = create_string(value, &s);
    if (*st != correct)
    {
        return NULL;
    }
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL)
    {
        delete_string(s);
        return NULL;
    }
    node->data = s;
    node->count = 0;
    node->left_child = NULL;
    node->right_child = NULL;

    return node;
}

Node *create_node_s(String *s, statements *st)
{
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL)
    {
        delete_string(s);
        *st = allocate_error;
        return NULL;
    }
    node->data = s;
    node->count = 1;
    node->left_child = NULL;
    node->right_child = NULL;
    *st = correct;
    return node;
}


void delete_node(Node **n)
{
    
    if (*n != NULL)
    {
        delete_node(&((*n)->left_child));
        delete_node(&((*n)->right_child));
        delete_string((*n)->data);
        free(*n);
    }
    *n = NULL;
}

statements add_node(Node *root, Node *new_node)
{
    if (compare_strings(root->data, new_node->data) == 0)
    {
        root->count = root->count + 1;
        delete_node(&new_node);
        return correct;
    }
    if (compare_strings(root->data, new_node->data) > 0)
    {
        if (root->right_child != NULL)
        {
            return add_node(root->right_child, new_node);
        }
        root->right_child = new_node;
        return correct;
    }
    if (compare_strings(root->data, new_node->data) < 0)
    {
        if (root->left_child != NULL)
        {
            return add_node(root->left_child, new_node);
        }
        root->left_child = new_node;
        return correct;
    }
    return runtime_error;
}

statements find_node(Node *root, String *val, Node **resulting)
{

    if (compare_strings(root->data, val) == 0)
    {
        *resulting = root;
        return correct;
    }
    if (compare_strings(root->data, val) > 0)
    {
        if (root->right_child != NULL)
        {
            return find_node(root->right_child, val, resulting);
        }
        return not_found;
    }
    if (compare_strings(root->data, val) < 0)
    {
        if (root->left_child != NULL)
        {
            return find_node(root->left_child, val, resulting);
        }
        return not_found;
    }
    return runtime_error;
}

int deep_node(Node *root)
{
    int a = 0, b = 0;
    if (root->right_child != NULL)
    {
        a = deep_node(root->right_child);
    }
    if (root->left_child != NULL)
    {
        b = deep_node(root->left_child);
    }
    return max(a, b) + 1;
}

int get_nodes_cnt(Node *root)
{
    int a = 0, b = 0;
    if (root->right_child != NULL)
    {
        a = deep_node(root->right_child);
    }
    if (root->left_child != NULL)
    {
        b = deep_node(root->left_child);
    }
    return 1 + a + b;
}

void fill(Node *root, Node ***where, int *pos)
{
    (*where)[*pos] = root;
    (*pos) = (*pos) + 1;

    if (root->right_child != NULL)
    {
        fill(root->right_child, where, pos);
    }
    if (root->left_child != NULL)
    {
        fill(root->left_child, where, pos);
    }
}

int compare_nodes(const void *a, const void *b)
{
    const Node *_a = (const Node *)a;
    const Node *_b = (const Node *)b;

    return _b->count - _a->count ;
}

void delete_nodes(Node **nds)
{
    int size = get_nodes_cnt(nds[0]);
    for (int i = 0; i < size; i++)
    {
        delete_node(&(nds[i]));
    }
    free(nds);
}
Node **get_sorted(Node *root, int *l, statements *stm)
{
    int size = get_nodes_cnt(root);
    Node **nodes = (Node **)malloc(sizeof(Node *) * size);
    if (nodes == NULL)
    {
        *stm = allocate_error;
        return NULL;
    }
    int pos = 0;
    fill(root, &nodes, &pos);
    qsort(nodes, size, sizeof(Node *), compare_nodes);
    *l = size;
    return nodes;
}
void print_node(Node *root)
{
    if (root == NULL) {return;}

    print_string(root->data);
    printf("> %d>\n", root->count);

}

void print_nodes(Node *root, int depth)
{
    if (root == NULL) {return;}
    for (int i = 0; i < depth; i++)
    {
        printf("\t");
    }
    print_string(root->data);

    printf("> %d>\n", root->count);
    print_nodes(root->left_child, depth + 1);
    print_nodes(root->right_child, depth + 1);
}


void trace_nodes (Node *root,  FILE *file)
{
    if (root == NULL){return;}
    fprintf(file, "%s>%d>\n", root->data->value, root->count);
    trace_nodes(root->left_child, file);
    trace_nodes(root->right_child, file);
}

bool backup_nodes(Node *root){
    FILE *file = fopen("node_tree.txt", "w");
    if (file == NULL){
        return false;
    }

    trace_nodes(root, file);
    fclose(file);
    return true;
}