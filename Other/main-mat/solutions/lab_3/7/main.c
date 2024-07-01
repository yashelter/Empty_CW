#include "program.c"

list main_list;
undo_stack stack;

void signal_handler(int x)
{
    if (x == SIGINT)
    {
        free_list(&main_list);
        delete_stack(&stack);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);

    if (argc != 2)
    {
        printf("Error : something invalid\n");
        return 1;
    }

    stack.size = 0;
    stack.buf = 0;
    stack.nodes = NULL;
    main_list.in_head = NULL;
    statements typo = parsing_input_data(argv[1], &main_list);
    if (typo != correct)
    {
        printf("Error (1): with code %d\n", typo);
        free_list(&main_list);
        return 0;
    }

    printf("Sucessfully starting...\n");

    // print_list(&main_list, "stdout");
    typo = run(&main_list, &stack);

    if (typo != correct)
    {
        printf("Error (2): with  code %d\n", typo);
        free_list(&main_list);
        delete_stack(&stack);
        return 0;
    }
    free_list(&main_list);
    delete_stack(&stack);
}