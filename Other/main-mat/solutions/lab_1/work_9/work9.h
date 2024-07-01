#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define FIXED_SIZE 5

#define RANGE_BEG 3
#define RANGE_END 5

enum input_statements
{
    input_correct,
    input_invalid,
    input_wrong_number
};

bool is_number(char *str)
{
    char *result;
    long number = strtol(str, &result, 10);
    if (*result != '\0')
    {
        return false;
    }
    return true;
}

enum input_statements check_input(int argc, char *argv[])
{
    if (argc != 3)
    {
        return input_invalid;
    }
    if (!is_number(argv[1]) || !is_number(argv[2]))
    {
        return input_wrong_number;
    }
    return input_correct;
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int get_random_value(int a, int b)
{
    if (a > b)
    {
        swap(&a, &b);
    }
    return a + rand() % (b - a + 1);
}

void fill_array_rand(int a, int b, int *mass, int length)
{
    for (int i = 0; i < length; i++)
    {
        mass[i] = get_random_value(a, b);
    }
}

void print_mass(int *mass, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("<< <%d> is : %d ", i+1, mass[i]);
    }
}

void swap_max_min_elements(int *mass, int size)
{
    int minimum = INT_MAX;
    int maximum = INT_MIN;

    int max_pos, min_pos;

    for (int i = 0; i < size; i++)
    {
        if (mass[i] > maximum)
        {
            maximum = mass[i];
            max_pos = i;
        }
        if (mass[i] < minimum)
        {
            minimum = mass[i];
            min_pos = i;
        }
    }
    swap(&mass[max_pos], &mass[min_pos]);
}

int compare(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

int get_nearest_bs(int *mass, int length, int x)
{
    int left = 0;
    int right = length - 1;

    while (left < right - 1)
    {
        int mid = (left + right) >> 1;
        if (mass[mid] > x)
        {
            right = mid;
        }
        else
        {
            left = mid;
        }
    }
    if (abs(mass[left] - x) > abs(mass[right] - x))
    {
        return mass[right];
    }

    return mass[left];
}

void generate_c(int *a, int *b, int l1, int l2, int *c)
{
    for (int i = 0; i < l1; i++)
    {
        c[i] = a[i] + get_nearest_bs(b, l2, a[i]);
    }
}