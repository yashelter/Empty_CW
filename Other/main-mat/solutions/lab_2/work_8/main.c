#include "work8.c"

int main()
{
    statements stm;
    char *result  = sum_nums(&stm, 10, 2, "9", "9");
    if (stm == correct){
        printf("Result is : %s\n", result);
        free(result);
    }
    else{
        printf("Error occured!");
    }
    result  = sum_nums(&stm, 10, 2, "00000000000000000000A00000000000000000000000A", "000000000000B00000000000000000000000");
    if (stm == correct){
        printf("Result is : %s\n", result);
        free(result);
    }
    else{
        printf("Error occured!");
    }
}