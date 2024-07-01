#include "work2.c"
#include <time.h>

int main()
{ 
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    /*demonstation 1*/
    double result = geometric_middle(5, 2.0, 3.0, 4.0, 5.0, 6.0);
    printf("%f \n", result);

    result = geometric_middle(0, 2.0, 3.0, 4.0, 5.0, 6.0);
    printf("%f \n", result);

    result = geometric_middle(3, 2.0, 3.0, 4.0, 5.0, 6.0);
    printf("%f \n", result);

    

    printf("\n\n");
    /*demostration 2 */
    printf("%Lf \n", b_pow_d(3.5, 3));

    printf("%Lf \n", b_pow_d(0, 0));

    printf("%Lf \n", b_pow_d(-1, 3));

    printf("%Lf \n", b_pow_d(9999999, 9999));
    printf("%Lf \n", b_pow_d(9999999, 999999999));
    /**/


    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Runned : %.2f seconds\n", cpu_time_used);
}