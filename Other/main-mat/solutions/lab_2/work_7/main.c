#include "work7.c"
#include <time.h>

int main ()
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    double eps =  0.00001;

    double d1 = dichotomy(2, 5.3, &ex_1, eps),
           d2 = dichotomy(-3.13, 3.13, &ex_2, eps),
           d3 = dichotomy(0, 2, &ex_3, eps);

    printf("Calculated x : %.8f\n", d1);
    printf("X in equation give: %.8f\n\n", ex_1(d1));
  
    printf("Calculated x : %.8f \n", d2);
    printf("X in equation give: %.8f\n\n", ex_2(d2));
    
    printf("Calculated x : %.8f \n", d3);
    printf("X in equation give: %.8f\n", ex_3(d3));

    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nRunned : %.2f seconds\n", cpu_time_used);

}