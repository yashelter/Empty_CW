#include "work3.c"


void print_cell(Cell *c)
{
    if (c == NULL) { return; }

    printf("\n");
    printf("For file :  %s \n", c->path);
    if (c->stm == all_nums)
    {
        int cnt = 1;
        char cu;
        FILE *file = fopen(c->path, "r");
        if (file == NULL) {return;}

        int stri = 1;

        while ((cu = fgetc(file)) != EOF)
        {
           printf("At string %d — at position %d\n", stri, cnt);
           if (cu == '\n'){
            stri += 1;
            cnt = 0;
           }
           cnt++;
        }
        return;
    }
    if (c->stm == correct)
    {
        printf("Count is : %d\n", c->cnt);
        for (int i = 0; i < c->cnt ; i++)
        {
            printf("At string %d — at position %d\n", c->string_ns[i], c->char_ns[i]);
        }
    }
    printf("\n");
}


int main()
{
    Cell **cells;
    int n = 2;
    statements stm = find_all_patterns(&cells, "\n", n, "in.txt", "in2.txt");
    if (stm == correct)
    {
        for (int i = 0; i < n; i++)
        {
            print_cell(cells[i]);
        }
        free_Cells(cells, n);
    }
    else
    {
        printf("Error occured or input invalid (empty file - invalid)\n");
    }
}