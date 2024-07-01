#include "../MonoBehavior.c"

// операция сложения с помощью бинарных операций
int plus(int num1, int num2)
{
    int res = 0, carry = 0;
    res = num1 ^ num2;
    carry = (num1 & num2) << 1;
    while (carry)
    {
        int tmp = res;
        res = res ^ carry;
        carry = (tmp & carry) << 1;
    }
    return res;
}

statements prepare(int base, int *mask, int *in_left)
{
    int two = 1;
    int pow = 0;
    while (two < base)
    {
        two = two << 1;
        pow = plus(pow, 1);
    }
    if (two == base)
    {
        *mask = plus(base, -1);
        *in_left = pow;
        return correct;
    }
    return invalid_input;
}

int negative(int n)
{
    return plus(~n, 1);
}
void reverse_string(char **string, int start, int length)
{
    int i = 0;
    int j = 0;
    for (i = start, j = plus(length, - 1); i < j; i=plus(i, 1), j=plus(j, -1))
    {
        int tmp = (*string)[i];
        (*string)[i] = (*string)[j];
        (*string)[j] = tmp;
    }
}

statements number_to_cc(int number, int base, char **result)
{
    int mask = 0, in_left = 0;
    char *answer = (char *)malloc(sizeof(char) * 2);
    char *ptr = answer;
    
    if (answer == NULL)
    {
        return allocate_error;
    }
    int _buffer = 2;
    int counter = 0;
    int start = 0;

    statements stm = prepare(base, &mask, &in_left);
    if (stm != correct)
    {
        free(answer);
        return stm;
    }

    if (number == 0)
    {
        answer[0] = '0';
        answer[1] = '\0';
        *result = answer;
        return correct;
    }

    if (number < 0)
    {
        number = negative(number);
        answer[counter] = '-';
        counter++;
        start = 1;
    }

    while (number > 0)
    {
        counter = plus(counter, 1);
        if (counter >= _buffer)
        {
            _buffer = _buffer << 1;
            char *temp = (char *) realloc(answer, sizeof(char) * _buffer);
            if (temp == NULL)
            {
                free(answer);
                return allocate_error;
            }
            answer = temp;
        }
        int digit = mask & number;
        number >>= in_left;

        answer[plus(counter, - 1)] = digit > 9 ? (char)(plus(plus(digit, - 10), 'A')) : (char)(plus(digit, '0'));
    }
   
    reverse_string(&answer, start, counter);
    answer[counter] = '\0';

    *result = answer;
    return correct;
}

