#include "work1.h"


int main(int argc, char *argv[])
{
    print_hint();
    statements stm = validate_input(argc, argv);

    if (stm != correct)
    {
        printf("Error: Invalid input\n");
        return 0;
    }

    char *path = argv[1];
    FILE *output = NULL, *input = NULL;

    stm = try_to_open(&output, path, "wb");

    if (stm != correct) { return 0; }

    stm = task1(output);
    fclose(output);
    if (stm != correct) { return 0; }


    stm = try_to_open(&output, path, "rb");

    if (stm != correct) { return 0; }


    stm = task2(output);

    if (stm != correct) { return 0; }
    
    fclose(output);


    stm = try_to_open(&input, path, "rb");

    if (stm != correct) { return 0; }

    printf("\n\nMoving pointer to 3 from start @uwu@\n\n");

    stm = task3(&input);

    if (stm != correct) { return 0; }

    char buffer[4];

    stm = task4(&input, buffer);

    if (stm != correct) { return 0; }


    printf("What will be in buffer :\n"); 
    // 1 5 9 2
    for (int i = 0; i < 4; i += 1) 
    {
        printf("%u ", buffer[i]);
    }
    printf("\n");
    fclose(input);
}