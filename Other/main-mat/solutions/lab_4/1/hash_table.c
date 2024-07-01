#include "../Base.c"

#define BUFFER_SIZE 256

typedef struct Node
{
    char *key;
    char *value;
    struct Node *next;
} Node;

typedef struct Hash_table
{
    unsigned int size;
    Node **table;
} Hash_table;

unsigned int get_hash(Hash_table *map, char *key)
{
    unsigned int hash = 0;
    while (*key)
    {
        // 62 -alpth
        hash = (hash * 62 + *key) % map->size;
        key++;
    }
    return hash;
}

void delete_table(Hash_table *hashtable)
{
    for (unsigned int i = 0; i < hashtable->size; i++)
    {
        Node *current = hashtable->table[i];
        while (current != NULL)
        {
            Node *next = current->next;
            free(current->key);
            free(current->value);
            free(current);
            current = next;
        }
    }
    free(hashtable->table);
    free(hashtable);
}

Hash_table *create_hash_table(unsigned int size)
{
    Hash_table *hashtable = (Hash_table *)malloc(sizeof(Hash_table));
    if (hashtable == NULL)
        return NULL;

    hashtable->size = size;
    hashtable->table = (Node **)calloc(size, sizeof(Node *));
    if (hashtable->table == NULL)
    {
        free(hashtable);
        return NULL;
    }
    return hashtable;
}

statements insert(Hash_table *hashtable, char *key, char *value)
{
    size_t len1 = strlen(value);
    size_t len2 = strlen(key);
    if (len1 > 0 && value[len1 - 1] == '\n')
    {
        value[len1 - 1] = '\0';
    }

    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        return allocate_error;
    }
    new_node->value = (char *)malloc(sizeof(char) * (len1 + 1));
    if (new_node->value == NULL)
    {
        free(new_node);
        return allocate_error;
    }
    strcpy(new_node->value, value);

    new_node->key = (char *)malloc(sizeof(char) * (len2 + 1));

    if (new_node->key == NULL)
    {
        free(new_node->value);
        free(new_node);
        return allocate_error;
    }
    strcpy(new_node->key, key);

    unsigned int index = get_hash(hashtable, key);

    new_node->next = hashtable->table[index];

    hashtable->table[index] = new_node;

    return correct;
}

bool in_alpthabet(const char symb)
{
    return (isalpha(symb) || isdigit(symb));
}

statements paste_to_string(char **string, const char *old, const char *new)
{
    char *str = *string;
    int lent = strlen(str), lenn = strlen(old), delta = -lenn + strlen(new);
    for (int i = 0; i < (lent - lenn); i++)
    {
        if ((strncmp(str + i, old, lenn) == 0) && (i == 0 || !in_alpthabet(*(str + i - 1))) && (i > lent - 1 || !in_alpthabet(*(str + i + 1))))
        {
            lent += delta;
            if (delta > 0)
            {
                char *temp = (char *)realloc(str, sizeof(char) * (lent + 1));
                if (temp == NULL)
                {
                    free(str);
                    return allocate_error;
                }
                str = temp;
            }

            memmove(str + i + strlen(new), str + i + lenn, strlen(str + i + lenn) + 1);
            memcpy(str + i, new, strlen(new));
            i += lenn - 1;
        }
    }
    *string = str;
    return correct;
}

void resize_map(Hash_table **hashtable, unsigned int new_size)
{
    Hash_table *new_table = create_hash_table(new_size);

    for (unsigned int i = 0; i < (*hashtable)->size; i++)
    {
        Node *current = (*hashtable)->table[i];
        while (current != NULL)
        {
            insert(new_table, current->key, current->value);
            current = current->next;
        }
    }

    delete_table(*hashtable);

    *hashtable = new_table;
}

statements better_inserting(Hash_table **hashtable, char *key, char *value)
{
    statements result = insert(*hashtable, key, value);

    unsigned int min_length = -1;
    unsigned int max_length = 0;

    for (unsigned int i = 0; i < (*hashtable)->size; i++)
    {
        Node *current = (*hashtable)->table[i];
        unsigned int chain_length = 0;
        while (current != NULL)
        {
            chain_length++;
            current = current->next;
        }

        if (chain_length > max_length)
        {
            max_length = chain_length;
        }

        if (chain_length < min_length)
        {
            min_length = chain_length;
        }
    }

    if (max_length >= 2 * min_length)
    {
        if ((*hashtable)->size < max_length)
        {
            resize_map(hashtable, (*hashtable)->size * 2);
        }
        else if ((*hashtable)->size > 2 * min_length)
        {
            resize_map(hashtable, (*hashtable)->size / 2);
        }
    }

    return result;
}

statements parsing_file(Hash_table **hashtable, char *input_filename, char *output_filename)
{
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL)
        return invalid_file;

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL)
    {
        fclose(input_file);
        return invalid_file;
    }

    char *line = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    if (line == NULL)
    {
        fclose(input_file);
        fclose(output_file);
        return allocate_error;
    }
    char *ptr = NULL;

    while (fgets(line, sizeof(char) * BUFFER_SIZE, input_file) != NULL)
    {
        ptr = strstr(line, "#define");
        if (ptr != NULL)
        {
            char def_name[BUFFER_SIZE];
            char value[BUFFER_SIZE];
            if (sscanf(ptr, "#define %255s %255[^\n]", def_name, value) == 2)
            {
                if (better_inserting(hashtable, def_name, value) != correct)
                {
                    fclose(input_file);
                    fclose(output_file);
                    return allocate_error;
                }
            }
            else
            {
                printf("Empty #define : %s\n", line);
            }
        }
        else
        {
            for (unsigned int i = 0; i < (*hashtable)->size; i++)
            {
                Node *current = (*hashtable)->table[i];
                while (current != NULL)
                {
                    statements stm = paste_to_string(&line, current->key, current->value);
                    if (stm != correct)
                    {
                        fclose(input_file);
                        fclose(output_file);
                        return stm;
                    }
                    current = current->next;
                }
            }
        }
        fprintf(output_file, "%s", line);
        if (line != NULL)
        {
            free(line);
        }
        line = (char *)malloc(sizeof(char) * BUFFER_SIZE);
        if (line == NULL)
        {
            fclose(input_file);
            fclose(output_file);
            return allocate_error;
        }
    }
    free(line);
    fclose(input_file);
    fclose(output_file);

    return correct;
}
