#include "parse.c"

int main(int argc, char *argv[])
{
    if (argc != 2){
        printf("Usage <%s> <filepath>\n", argv[0]);
        return 1;
    }
     //print_memory_cell(cells.cells[bin_search(&cells, "af")]);
    printf("exit code = %d\n", run(argv[1]));
}