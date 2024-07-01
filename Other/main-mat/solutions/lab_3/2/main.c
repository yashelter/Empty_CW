#include "app.c"
#include <time.h>

int main()
{
    srand(time(NULL));

    double myPoints[] = {1.0, 2.0, 3.0}; // ваш массив точек

    Vector e1 = get_rand_vector(3), e2 = get_rand_vector(3), e3 = get_rand_vector(3);
    print_vector(&e1);
    double p = 2.0;

    double matrix[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9};

    Vector **result = task(3, 3, e1, e2, e3, &norm_first, NULL, &norm_second, &p, &norm_third, matrix);

    if (result == NULL)
    {
        printf("Error : Something went wrong\n");
        delete_vector(&e1);
        delete_vector(&e2);
        delete_vector(&e3);

        return 1;
    }
    for (int i = 0; i < 3; i++)
    {
        printf("For norm function %d:\n", i + 1);
        for (int j = 0; result[i][j].pts != NULL; j++)
        {
            printf("Vector %d is : ", j + 1);
            print_vector(&(result[i][j]));
        }
        free(result[i]);
    }

    delete_vector(&e1);
    delete_vector(&e2);
    delete_vector(&e3);

    free(result);

    return 0;
}
