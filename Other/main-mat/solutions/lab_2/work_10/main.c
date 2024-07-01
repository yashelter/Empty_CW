#include "work10.c"


int main() {
    double a = 4.0, x = 6.0;
    int degree = 3;

    double *mas1 = (double *) malloc(sizeof(double) * 3);

    mas1[0] = 2;
    mas1[1] = 3;
    mas1[2] = 1;

    double res = calculate(x, 3, mas1);


    printf("%lf \n", res);
    printf("=========================\n\n");

    double *mas2 = compute_coefficients(1, a, 3, 2.0, 3.0, 1.0);
    for (int i = 0; i < 3; i++)
    {
       printf("%lf ", mas2[i]);
    }
    
    res = calculate(x - a, 3, mas2);
    printf("\n%lf \n", res);

    free(mas1);
    free(mas2);
    
    return 0;
}