#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <ctype.h>

const long double MIN_EPSILON_VAL = 1e-9,
                  MAX_EPSILON_VAL = 1.0;

const unsigned long long MAX_LL_VAL = 1e18, MAX_DIG_LL = 18;

// check fractional number to correctivility
enum fractional_checkmark
{
    fractional_correct,
    fractional_not_digit,
    fractional_overflow,
    fractional_underflow
};

enum fractional_checkmark check_fractional(long double num)
{
    if (num < MIN_EPSILON_VAL || num == 0) { return fractional_underflow; }
    if (num >= LDBL_MAX || 2 * num * num >= LDBL_MAX) { return fractional_overflow; }
    return fractional_correct;
}


enum fractional_checkmark get_fractional(char *str_number, long double *resulting_number)
{
    char *endptr;
    *resulting_number = strtold(str_number, &endptr);
    
    if (*str_number != '\0' && *endptr == '\0') { return check_fractional(*resulting_number); }

    resulting_number = NULL;
    return fractional_not_digit;

}

// uzs = under zero support
enum fractional_checkmark check_fractional_uzs(long double num)
{
    if (num == 0){return fractional_underflow;}
    if (num >= LDBL_MAX || 2 * num * num >= LDBL_MAX) { return fractional_overflow; }
    return fractional_correct;
}

enum fractional_checkmark get_fractional_uzs(char *str_number, long double *resulting_number)
{
    char *endptr;
    *resulting_number = strtold(str_number, &endptr);
    if (*str_number != '\0' && *endptr == '\0') { return check_fractional_uzs(*resulting_number); }
    resulting_number = NULL;
    return fractional_not_digit;

}
// check flags to correctivitily

enum flag_chemark
{
    flag_non_flag,
    flag_not_servicing,
    flag_correct
};

enum flag_chemark check_flag(int argc, char* argv[], char *flag)
{
    if (argc < 2) {return flag_non_flag;}
    if (strlen(argv[1]) != 2 || (argv[1][0] != '-' && argv[1][0] != '/')) {return flag_non_flag; }
    if (argv[1][1] != 'q' && argv[1][1] != 'm' && argv[1][1] != 't') { return flag_not_servicing; }
    *flag = argv[1][1];
    return flag_correct;
}


// check ll numbers and get
enum digit_status
{
    digit_status_correct,
    digit_status_overflow,
    digit_status_non_digit,
    digit_status_fractional,
    digit_status_under_zero,
    digit_status_zero
};

enum digit_status char_to_ll(char* number, unsigned long long * result)
{
    int number_size = strlen(number);
    if (number_size > MAX_DIG_LL) { return digit_status_overflow; }

    *result = 0;
    if (number[0] == '-') { return digit_status_under_zero; }

    for (int i = 0; i < number_size; i++)
    {
        if (number[i] == '.' || number[i] == ',' )
        {
            return digit_status_fractional;
        }
        if (isalpha(number[i]) || !isalnum(number[i]))
        {
            return digit_status_non_digit;
        }
        *result = (*result * 10) + (unsigned long long) (number[i] - '0');
    }
    if (*result == 0) {return digit_status_zero; }
    return digit_status_correct;
}

enum digit_status get_ll_numbers(int argc, char* argv[], unsigned long long * num1, unsigned long long * num2)
{  
    enum digit_status state1 = char_to_ll(argv[2], num1);
    enum digit_status state2 = char_to_ll(argv[3], num2);
    if (state1 != digit_status_correct) { return state1;}
    if (state2 != digit_status_correct) { return state2;}
    return digit_status_correct;
}

// flag <-m>

bool check_for_separable(long long first_number, long long second_number)
{
    return (first_number % second_number == 0);
}

// flag <-t>

void swap(long double *a, long double *b) {
    long double temp = *a;
    *a = *b;
    *b = temp;
}

void sort_3_ld(long double* a, long double *b, long double *c)
{
    if (c < a) { swap(a, c); }
    if (c < b) { swap(b, c); }
    if (b < a) { swap(a, b); }
}

bool is_triangle(long double eps, long double a, long double b, long double c)
{
    char* end;
    sort_3_ld(&a, &b, &c);
    if((a * a + b * b) <= c * c + eps && (a * a + b * b) >= c * c - eps) { return true; }
   return false;
}


// flag <-q>
enum q_flag
{
    q_one_x,
    q_two_x,
    q_no_x,
    q_error
};
void solve_quadratic(long double eps, long double a, long double b, long double c) {

    long double discriminant = b * b - 4 * a * c;
    if (discriminant < 0 && discriminant + eps >= 0 )
    {
        b += eps;
        discriminant = 0; 
    }

    if (discriminant > 0) 
    {
        long double x1 = (-b + sqrt(discriminant)) / (2 * a);
        long double x2 = (-b - sqrt(discriminant)) / (2 * a);
        printf("The answer for koeff a = <%Lf>, b = <%Lf>, c = <%Lf> is : <%Lf> and <%Lf>\n", a,b,c,x1,x2);
    } 
    else if (discriminant == 0) {
        long double x = -b / (2 * a);
       printf("The answer for koeff a = <%Lf>, b = <%Lf>, c = <%Lf> is : <%Lf>\n", a,b,c,x);
    } 
    printf("There no answer for koeff a = <%Lf>, b = <%Lf>, c = <%Lf>\n", a,b,c);
}
int compare(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

void swaps (long double * mas, int i, int j){
    swap(mas + i, &mas[j]); // 
}
bool next_perm(long double *a, int n){
    int j = n - 2;
    while (j != -1 && a[j] >= a[j+1]) j--;
    if (j == -1) return false;
    int k = n - 1;
    while (a[j] >= a[k]) k--;
    swaps(a, j, k);
    int l = j + 1, r = n - 1;
    while (r - l > 0){
        swaps(a, l++, r--);
    }
    return true;
}
void print_quadratic(int argc, char* argv[])
{
    long double e, a, b, c;
    get_fractional(argv[2], &e);
    get_fractional_uzs(argv[3], &a);
    get_fractional_uzs(argv[4], &b);
    get_fractional_uzs(argv[5], &c);

    // all permutations 
    long double mas[3] = {a, b, c};
    qsort(mas, 3, sizeof(long double), compare);

    solve_quadratic(e, mas[0], mas[1], mas[2]);

    while (next_perm(mas, 3)) {
            solve_quadratic(e, mas[0], mas[1], mas[2]);
    }
}

// check input to correctivitily

enum input_checkmark
{
    input_correct,
    input_invalid_epsilon,
    input_invalid_flag,
    input_invalid_cnt_param,
    input_invalid_input,
    invalid_param
};


enum input_checkmark check_input_validate_n_run(int argc, char* argv[], char *flag)
{
    if (argc <= 2) { return input_invalid_input; }
    enum flag_chemark flag_checkpoint = check_flag(argc, argv, flag);
     
    switch (flag_checkpoint)
    {
        case flag_non_flag:
            return input_invalid_cnt_param;
            break;
        case flag_not_servicing:
            return input_invalid_flag;
            break;
        case flag_correct:
            break;
        default:
            return input_invalid_input;
    }
    
    switch (*flag)
    {
    case 'q':
        if (argc != 6)
        {
            printf("For this flag needed more numbers");
            return input_invalid_cnt_param;
        }
        long double e, a, b, c;
        enum fractional_checkmark state1 = get_fractional(argv[2], &e);
        enum fractional_checkmark state2 = get_fractional_uzs(argv[3], &a);
        enum fractional_checkmark state3 = get_fractional_uzs(argv[4], &b);
        enum fractional_checkmark state4 = get_fractional_uzs(argv[5], &c);
        if (state1 != fractional_correct || state2 != fractional_correct || 
            state3 != fractional_correct || state4 != fractional_correct)
            {
                return input_invalid_input;
            }
        
        return input_correct;
        break;
    case 'm':
        if (argc != 4)
        {
            printf("For this flag needed more numbers");
            return input_invalid_cnt_param;
        }
        unsigned long long num1, num2;
        enum digit_status nums_state = get_ll_numbers(argc, argv, &num1, &num2);
        if (nums_state != digit_status_correct) // there can check all situations
        {
            printf("Error: Check input paramers, something wrong\n");
            return input_invalid_input;
        }
        return input_correct;
        break;
    case 't':
    if (argc != 6)
        {
            printf("For this flag needed more numbers");
            return input_invalid_cnt_param;
        }
        // checker for numbers
        long double num;
        
        enum fractional_checkmark n1 = get_fractional(argv[2], &num),
                                  n2 = get_fractional(argv[3], &num),
                                  n3 = get_fractional(argv[4], &num),
                                  n4 = get_fractional(argv[5], &num);

        if (n1 != fractional_correct || n2 != fractional_correct || n3 != fractional_correct || n4 != fractional_correct)
        {
            printf("Error: Check input paramers, something wrong with fractional numbers\n");
            return input_invalid_input;
        }

        return input_correct;
        break;
        break;
    default:
        break;
    }
    return input_invalid_input;
    
}