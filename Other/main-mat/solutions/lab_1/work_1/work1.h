#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_SIZE_LL 18
#define MAX_LIMIT_RANGE 100

// input's checks

enum input_status
{
    input_status_no_parameters,
    input_status_invalid_type,
    input_status_invalid_count,
    input_status_correct
};

enum digit_status
{
    digit_status_correct,
    digit_status_overflow,
    digit_status_non_digit,
    digit_status_fractional,
    digit_status_under_zero
};

enum digit_status from_str_to_ll(char* number, unsigned long long * result)
{  
    int number_size = strlen(number);

    if (number_size > MAX_SIZE_LL)
    {
        return digit_status_overflow;
    }

    *result = 0;
    if (number[0] == '-'){
        return digit_status_under_zero;
    }

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
        
        *result = (*result * 10) + (long long) (number[i] - '0');
    }
    return digit_status_correct;
}

//======================================================================

// flag <-h>

enum separable_digits_in_range{
    not_have_sdr,
    correct_sdr,
    memory_err_sdr
};

enum separable_digits_in_range check_separable_digits_in_range(unsigned long long number, unsigned char *** array_answer )
{
    if (number > MAX_LIMIT_RANGE || number <= 0) { return not_have_sdr; }
    int size = MAX_LIMIT_RANGE / number;
    //**array_answer =  (unsigned char *) malloc(sizeof(unsigned char) * size);
    **array_answer =  (unsigned char *) malloc(size);

    if (**array_answer == NULL) { return memory_err_sdr;}
    
    for (int i = number, j = 0; i <= MAX_LIMIT_RANGE; i+=number, j++)
    {
        (**array_answer)[j] = (unsigned char) i;
    }
    return correct_sdr;
}


void print_separable_digits_in_range(unsigned long long number, unsigned char * array_answer, enum separable_digits_in_range state)
{
    switch(state) 
    {
        case not_have_sdr:
            printf("Error: number <%llu> dont have these numbers :( \n", number);
            break;
        case correct_sdr:
            printf("Print of separable digits in range 1..100 for number <%llu> : \n", number);
            int size = MAX_LIMIT_RANGE / number;
            for (int i = 0; i < size; i++){
                if (i != size - 1){
                    printf("%d, ", array_answer[i]);
                }
                else{
                    printf("%d.", array_answer[i]);
                }
                if ((i+1) % 15 == 0){
                    printf("\n");
                }
    }
            break;
        case memory_err_sdr:
            printf("Error: impossible to create array\n");
            break;
        default:
            break;
    } 
  
}
//======================================================================


// flag <-p>
enum number_value
{
    is_prime,
    not_prime
};


enum number_value check_for_primary(unsigned long long number) {
    if (number == 0) { return not_prime; }
    for (int check_num = 2; check_num * check_num <= number; check_num++) {
        if (number % check_num == 0) { return not_prime; }
    }
    return is_prime;
}

void print_primary_check_result(enum number_value num_val, unsigned long long number)
{
    switch(num_val) 
    {
        case is_prime:
            printf("Entered number: <%llu> is prime\n", number);
            break;
        case not_prime:
            printf("Entered number: <%llu> is composite\n", number);
            break;
        default:
            break;
    } 
}
//======================================================================


//flag <-s>

unsigned char* get_split_by_digits(char* number)
{  
    int number_size = strlen(number);
    unsigned char * resulting_array = (unsigned char * )malloc(sizeof(unsigned char) * (number_size + 1));

    for (int i = 0; i < number_size; i++)
    {
        resulting_array[i] = (number[i]);
    }
    resulting_array[number_size] = '\0';
    return resulting_array;
}


void print_digits(const unsigned char* array)
{
    printf("Splited entered number by digits is : ");
    while (*array != '\0')
    {
        printf("%c ", *array);
        array++;
    }
    free(array);
    printf("\n");
}

//======================================================================

// flag <-e>

enum table_possibillity{
    table_valid,
    table_invalid_input
};

enum table_possibillity validate_input(unsigned long long *number)
{
    if (*number < 1 || *number > 10) {return table_invalid_input;}
    return table_valid;
}

void print_table_of_powers(enum table_possibillity can_build, unsigned long long *number)
{
    switch (can_build)
    {
    case table_invalid_input:
        printf("Was enetered incorrect limit, please check that it stays in 1 to 10 to solve table of powers\n");
        break;
    case table_valid:
        printf("Table of powers from 1 to 10, base range: 1 to %llu", *number);
        for (int bases = 1; bases <= 10; bases++) 
        {
            unsigned long long cur_base = (unsigned long long)(cur_base);
            printf("\n\nFor base %d:  ", bases);
            for (int degree = 1; degree <= *number; degree++)
            {
                printf("%d^%d=%llu; ", bases, degree, cur_base);
                cur_base *= bases;
            }
        }
        printf("\n");
        break;
    default:
        printf("Unknown status\n");
        break;
    }
}

//======================================================================

// flag <-a>
enum sum_checkmarks{
    sum_overflow,
    sum_correct
};
enum sum_checkmarks sum_of_numbers(unsigned long long *number, unsigned long long * result)
{
    unsigned long long sum = (*number * (*number + 1)) / 2;
    if (sum < *number){
        return sum_overflow;
    }
    *result = sum;
    return sum_correct;
}

void print_sum_of_natural_numbers (enum sum_checkmarks code, unsigned long long *sum)
{
    switch (code)
    {
        case sum_overflow:
            printf("Sum, while solving sum of natural numbers, become more than it could be - Overflow\n");
            break;
        case sum_correct:
            printf("Resulting sum for all natural numbers 1...n: <%llu>\n", *sum);
            break;
    }
}
//======================================================================

// flag <-f>

enum factorial_status_code
{
    fsc_correct,
    fsc_overflow,
    fsc_invalid_input,
    fsc_solving
};

enum factorial_status_code factorial(unsigned long long *number, unsigned long long *result)
{
    if (*number >= 20)  {  return fsc_invalid_input; } 
    if (*number <= 1)
    {
        *result = 1;
        return fsc_correct;
    }
    *number -= 1;
    enum factorial_status_code now_position = factorial(number, result);
    *number += 1;

    if (now_position == fsc_correct)
    {
        if ((*result) * (*number) >= ULONG_MAX || (*result) * (*number) <= 0) {
            return fsc_overflow;
        }
        *result *= (*number);
    }
    return now_position;
}

void print_factorial (enum factorial_status_code status, unsigned long long *number, unsigned long long *factorial)
{
    switch(status)
    {
        case fsc_overflow: 
            printf("Became an overflow, while solving factorial"); 
            break; 
        case fsc_invalid_input: 
            printf("Please enter number lower than 20 for solving factorial"); 
            break; 
        case fsc_correct: 
            printf("Factorial of number <%llu> is <%llu>\n", *number, *factorial); 
            break;
         case fsc_solving:
            break; 
    }
}
//======================================================================

// send to correct functions
void make_call(char status, char* str_number, unsigned long long ll_number)
{
    switch (status)
    {
    case 'h':
        unsigned char * array = NULL;
        unsigned char **arr = &array;
        print_separable_digits_in_range(ll_number, *arr, check_separable_digits_in_range(ll_number, &arr));
        free(arr);
        break;
    case 'p':
        print_primary_check_result(check_for_primary(ll_number), ll_number);
        break;
    case 's':
        print_digits(get_split_by_digits(str_number));
        break;
    case 'e':
        print_table_of_powers(validate_input(&ll_number), &ll_number);
        break;
    case 'a':
        unsigned long long sum_res = 0;
        print_sum_of_natural_numbers(sum_of_numbers(&ll_number, &sum_res), &sum_res);
        break;
    case 'f':
        unsigned long long factor = 0;
        print_factorial(factorial(&ll_number, &factor), &ll_number, &factor);
        break;
    default:
        break;
    }
}

enum inputs_status{
    inp_correct,
    inp_wrong
};

enum inputs_status validate_input_all(int argc){
    if (argc == 3){
        return inp_correct;
    }
    return inp_wrong;
}

