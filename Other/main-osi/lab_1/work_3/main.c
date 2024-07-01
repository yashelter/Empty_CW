#include "work3.h"

int main(int argc, char *argv[])
{
    //print_hint();
    statements stm = check_input(argc, argv);

    FILE *input = NULL, *output = NULL;
    if (stm != correct) {print_err(stm); return 0;}

    stm = try_open_file(&input, argv[1], "r");
    if (stm != correct) { print_err(stm); return 0; }

    stm = try_open_file(&output, argv[2], "w");
    if (stm != correct) {fclose(input); print_err(stm); return 0;}


    stm = copy(input, output);
    if (stm != correct) {fclose(input); fclose(output); print_err(stm); return 0;}

    printf("Succesfully copied\n");

    fclose(input);
    fclose(output);
}