#include "../Base.c"

typedef struct MemoryCell
{
    char *name;
    int val;
} MemoryCell;

typedef struct Cells
{
    MemoryCell **cells;
    int len;
    int buf;
} Cells;

MemoryCell *create_cell(const char *name, int value)
{
    if (name == NULL)
    {
        return NULL;
    }
    int l = strlen(name);
    MemoryCell *cell = (MemoryCell *)malloc(sizeof(MemoryCell));
    if (cell == NULL)
    {
        return NULL;
    }
    cell->name = (char *)malloc(sizeof(char) * (l + 1));
    if (cell->name == NULL)
    {
        free(cell);
        return NULL;
    }
    strcpy(cell->name, name);
    cell->val = value;
    return cell;
}

bool is_latin(const char *s)
{
    int l = strlen(s);
    for (int i = 0; i < l; i++)
    {
        if (!isalpha(s[i])){
            return false;
        }
    }
    return true;
    
}

bool is_number(const char *s)
{
    int l = strlen(s);
    for (int i = 0; i < l; i++)
    {
        if (!isdigit(s[i])){
            return false;
        }
    }
    return true;
    
}
void delete_cell(MemoryCell **cell)
{
    if (*cell == NULL)
    {
        return;
    }
    free((*cell)->name);
    free(*(cell));
    *cell = NULL;
}

void delete_cells(Cells *cells)
{
    for (int i = 0; i < cells->len; i++)
    {
        delete_cell(&(cells->cells[i]));
    }
    free(cells->cells);
}

int comparator(const void *a, const void *b)
{
    MemoryCell *left = *(MemoryCell **)a;
    MemoryCell *right = *(MemoryCell **)b;
    return strcmp(left->name, right->name);
}

statements add_cell(Cells *cells, MemoryCell *cell)
{
    if (cells->len + 1 > cells->buf)
    {
        cells->buf = cells->buf * 2;
        MemoryCell **temp = (MemoryCell **)realloc(cells->cells, (sizeof(MemoryCell *) * cells->buf));
        if (temp == NULL)
        {
            delete_cells(cells);
            return allocate_error;
        }
        cells->cells = temp;
    }
    (cells->cells)[(cells->len++)] = cell;
    qsort(cells->cells, cells->len, sizeof(MemoryCell *), comparator);
    return correct;
}

// return index
int bin_search(Cells *cells, const char *name)
{
    int a = -1, b = cells->len;
    while (b - a > 1)
    {
        int mid = (a + b) / 2;
        int mval = strcmp((cells->cells[mid])->name, name);
        //printf("%d, %d\n", mval, mid);
        if (mval < 0)
        {
            a = mid;
        }
        else if (mval > 0)
        {
            b = mid;
        }
        else
        {
            return mid;
        }
    }
    return -1;
}

void print_memory_cell(MemoryCell *cell)
{
    printf("%s = %d\n", cell->name, cell->val);
}

void print_vals(Cells *cells)
{
    printf("\nAll values of cells (%d) :\n", cells->len);
    for (int i = 0; i < cells->len; i++)
    {
        print_memory_cell(cells->cells[i]);
    }
    printf("\n");
}