#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

typedef struct
{
    double *pts;
    int size;
} Vector;

typedef double (*norm_pointer)(Vector *, void *);

double norm_first(Vector *vec, void *param)
{
    double max_coord = 0;
    for (int i = 0; i < vec->size; i++)
    {
        double coord = fabs(vec->pts[i]);
        if (coord > max_coord)
            max_coord = coord;
    }
    return max_coord;
}

double norm_second(Vector *vec, void *param)
{
    double p = *(double *)param;
    double sum = 0;
    for (int i = 0; i < vec->size; i++)
    {
        sum += pow(fabs(vec->pts[i]), p);
    }
    return pow(sum, 1.0 / p);
}

double norm_third(Vector *vec, void *param)
{
    double *matrix = (double *)param;
    int n = vec->size;

    double sum = 0;
    for (int i = 0; i < n; i++)
    {
        double temp = 0;
        for (int j = 0; j < n; j++)
        {
            temp += vec->pts[j] * matrix[i * n + j];
        }
        sum += (fabs(temp) * vec->pts[i]);
    }
    return sum;
}

void print_vector(Vector *v)
{
    printf("{");
    for (int i = 0; i < v->size; i++)
    {
        printf("%.2f", v->pts[i]);
        if (i != v->size - 1)
        {
            printf(", ");
        }
    }
    printf("}\n");
}

void delete_vector(Vector *v){
    free(v->pts);
}
// returns vector of length n
Vector get_rand_vector(int n)
{
    Vector v;
    double *mas = (double *)malloc(sizeof(double) * n);
    if (mas == NULL)
    {
        return v;
    }
    for (int i = 0; i < n; i++)
    {
        mas[i] = (rand() % 10) * 1.0;
        // printf("i = %f", mas[i]);
    }
    // printf("\n");
    v.pts = mas;
    v.size = n;
    return v;
}

Vector **task(int num_vectors, int num_norms, ...)
{
    va_list args;
    va_start(args, num_norms);

    Vector **resulting = (Vector **)malloc(num_norms * sizeof(Vector *));

    if (resulting == NULL)
    {
        va_end(args);
        return NULL;
    }

    Vector *vectors = (Vector *)malloc(num_vectors * sizeof(Vector));
    if (vectors == NULL)
    {
        free(resulting);
        va_end(args);
        return NULL;
    }

    for (int i = 0; i < num_vectors; i++)
    {
        vectors[i] = va_arg(args, Vector);
    }
    //
    for (int i = 0; i < num_norms; i++)
    {
        norm_pointer func = va_arg(args, norm_pointer);
        void *param_for_norm = va_arg(args, void *);

        double max_norm = -1.0;
        Vector *temp = (Vector *)malloc((num_vectors + 1) * sizeof(Vector));

        if (temp == NULL)
        {
            free(vectors);
            for (int k = 0; k < i; k++)
            {
                free(resulting[k]);
            }
            free(resulting);
            return NULL;
        }

        int counter = 0;

        for (int j = 0; j < num_vectors; j++)
        {
            Vector vec = vectors[j];
            double norm = func(&vec, param_for_norm);

            if (norm > max_norm)
            {
                max_norm = norm;
                counter = 0;
            }

            if (norm == max_norm)
            {
                temp[counter] = vec;
                counter++;
            }
        }
        temp[counter].pts = NULL;

        Vector *ptr = realloc(temp, (counter + 1) * sizeof(Vector));
        if (ptr == NULL)
        {
            free(vectors);
            free(temp);
            for (int k = 0; k < i; k++)
            {
                free(resulting[k]);
            }
            free(resulting);
            return NULL;
        }
        resulting[i] = ptr;
    }

    va_end(args);
    free(vectors);

    return resulting;
}
