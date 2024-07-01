#include "work9.h"

void print_hint()
{
    printf("==Program made by Gubarev Mikhail==\n\n");
    printf("=============A few info: ==========\n");
    printf("This program unsuports flags, just two numbers \n");
    printf("Numbers a, b are forming range [a ... b] \n");
    printf("\n\n");
    printf(">>> Program Output <<<\n\n");
}

int main(int argc, char *argv[])
{
    print_hint();

    srand(time(NULL));
    int random[FIXED_SIZE];

    enum input_statements stm = check_input(argc, argv);

    if (stm == input_correct)
    {
        fill_array_rand(strtol(argv[1], NULL, 10), strtol(argv[2], NULL, 10), random, FIXED_SIZE);

        printf("\n<< Random-generated array: \n");
        print_mass(random, FIXED_SIZE);

        swap_max_min_elements(random, FIXED_SIZE);
        printf("\n<< Random-generated array with changed max and min: \n\n");
        print_mass(random, FIXED_SIZE);
    }
    else
    {
        printf("\nError : something went wrong, check input or your pc\n");
    }
    printf("\n\n");

    int l1 = get_random_value(RANGE_BEG, RANGE_END), l2 = get_random_value(RANGE_BEG, RANGE_END);

    int *array_a = (int *)malloc(sizeof(int) * l1);
    int *array_b = (int *)malloc(sizeof(int) * l2);
    int *array_c = (int *)malloc(sizeof(int) * l1);

    if (array_a == NULL || array_b == NULL || array_c == NULL)
    {
        printf("\n<< Error : can't make arrays, something wrong");
        free(array_a);
        free(array_b);
        free(array_c);

        return 0;
    }
    else
    {
        fill_array_rand(-1000, 1000, array_a, l1);
        fill_array_rand(-1000, 1000, array_b, l2);

        printf("\n\n<< Random-generated array A: \n\n");
        print_mass(array_a, l1);

        printf("\n\n<< Random-generated array B: \n\n");
        print_mass(array_b, l2);

        qsort(array_b, l2, sizeof(int), compare);

        generate_c(array_a, array_b, l1, l2, array_c);
       
        printf("\n\n<< Array generated from A and B by rules: \n\n");
        print_mass(array_c, l1);

        free(array_a);
        free(array_b);
        free(array_c);
    }
    return 0;
}