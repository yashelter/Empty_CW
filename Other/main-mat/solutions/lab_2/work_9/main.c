#include "work9.c"

int main() 
{
    double numbes[] = {0.125, 0.5, 0.25, 0.1, 0.3};
    int *answ;

    statements stm = check_representation(&answ, 10, 5, 0.125, 0.5, 0.25, 0.1, 0.3);
    if (stm == correct)
    {
        for (int i = 0; i < 5; i++)
        {
            printf("Representation %.5lf in base %d: %s\n", numbes[i], 10, (answ[i] == 0) ? "Finite" : "Infinite");
        }
        free(answ);
        answ = NULL;
    }
    double numbes2[] = {0.875, 0.125, 0.375, 0.5, 0.04};
    stm = check_representation(&answ, 8, 5, 0.875, 0.125, 0.375, 0.5, 0.04);
    if (stm == correct)
    {
        for (int i = 0; i < 5; i++)
        {
            printf("Representation %.5lf in base %d: %s\n", numbes2[i], 8, (answ[i] == 0) ? "Finite" : "Infinite");
        }
        free(answ);
        answ = NULL;
    }
    //check_representation(5, 5, 0.4, 0.5, 0.25, 0.1, 0.3);
    
    return 0;
}