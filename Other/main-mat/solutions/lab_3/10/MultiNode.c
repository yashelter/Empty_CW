#include "../4/String.c"

#define NODE_CHILDREN 8

typedef struct Node Node;

struct Node
{
    Node **children;

    Node *master;
    String *data;
    int childCount;
    int _buffer;
};


statements create_node(String * value, Node **result)
{
    Node *n = (Node *)malloc(sizeof(Node));
    if (n == NULL)
    {
        return allocate_error;
    }

    Node **children = (Node **)malloc(sizeof(Node *) * NODE_CHILDREN);
    if (children == NULL)
    {
        free(n);
        return allocate_error;
    }
    n->children = children;
    n->childCount = 0;
    n->data = value;
    n->_buffer = NODE_CHILDREN;
    n->master = NULL;
    *result = n;
    return correct;
}

statements add_child(Node *master, Node *slave)
{
    if (master ->childCount + 1 >= master->_buffer){
        master->_buffer = master->_buffer * 2;
        Node **temp = (Node **)realloc(master->children, sizeof(Node *) * master->_buffer);
        if (temp == NULL){
            free(master->children);
            return allocate_error;
        }
        master->children = temp;
    }
    (master->children)[master->childCount] = slave;
    master->childCount = master->childCount +1;
    slave->master = master;
    return correct;
}

void delete_node(Node *root)
{
    free(root->children);
    root->children = NULL;
    delete_string(root->data);
    root->master = NULL;
    free(root);
}
void delete_nodes(Node *root){
    for (int i = 0; i < root->childCount; i++)
    {
        delete_nodes((root->children)[i]);
    }
    delete_node(root);
}
void print_nodes(Node *root, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        printf("\t");
    }
    print_string(root->data);
    printf(">\n");
    for (int i = 0; i < root->childCount; i++)
    {
        print_nodes((root->children)[i], depth + 1);
    }
}
void fprint_nodes(FILE *out, Node *root, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        fprintf(out, "\t");
    }
    fprint_string(out, root->data);
    fprintf(out, ">\n");
    for (int i = 0; i < root->childCount; i++)
    {
        fprint_nodes(out, (root->children)[i], depth + 1);
    }
}