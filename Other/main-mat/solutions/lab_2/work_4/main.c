#include "work4.c"

int main() 
{

    printf("Is Convex: %d\n", isConvexPolygon(6, -1.0, 1.0, 2.0, 4.0, 3.0, 3.0, 4.0, 1.0, 3.0, -1.0, 1.0, -2.0)); // Должно быть 1 
    printf("Is Convex: %d\n", isConvexPolygon(4, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0)); // Должно быть 1 (выпуклый квадрат)

    printf("Polynomial Value: %lf\n", polynomialValue(2, 1, 2.0, -3, 2)); // 2 ( x^2 - 3x + 2 в точке x=2)

    return 0;
}