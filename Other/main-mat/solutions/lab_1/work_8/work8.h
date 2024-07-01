#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

#define MAX_LENGTH 20

enum input_statements
{
    input_correct,
    input_wrong_base,
    input_invalid,
    input_wrong_length,
    input_wrong_file
};


int char_digit_to_number(char digit)
{
    if ('0' <= digit && digit <= '9') { return digit - '0'; }
    if ('A' <= digit && digit <= 'Z') {  return digit - 'A' + 10; }
    if ('a' <= digit && digit <= 'z') {  return digit - 'a' + 10; }
    return -1; 
}

enum input_statements ss_to_base_10(char* str, int base, long long *result) 
{
    long long number = 0;
    long long remain_part = 1;

    long long sign = 1;
    int lwst = 0;
    
    if (str[0] == '-')  { sign = -1; lwst = 1; }

    if (strlen(str) >= MAX_LENGTH) {return input_wrong_length; } 

    for (int i = strlen(str) - 1; i >= lwst; i--) 
    {
        int num = char_digit_to_number(str[i]);
        if (num == -1 || num > base - 1) { return input_invalid; }

        number = number + num * remain_part;
        remain_part *= base;
    }

    number *= sign;
    *result = number;

    return input_correct;
}


int find_min_base(char c)
{   
    if ('0' <= c && c <= '9') { return c - '0' + 1; }
    if ('A' <= c && c <= 'Z') {  return c - 'A' + 11; } 
    if ('a' <= c && c <= 'z') {  return c - 'a' + 11; } 
    return -1;
}

enum input_statements check_input(int argc, char* argv[], FILE **input, FILE **output)
{
    if (argc != 3) { return input_invalid; }

    errno_t err = fopen_s(input, argv[1], "r");
    if (err != 0 || input == NULL) { return input_wrong_file; }

    err = fopen_s(output, argv[2], "w");
    if (err != 0 || output == NULL) { fclose(*input); return input_wrong_file; }

    return input_correct;
} 

void close_opened_file(FILE *file) { if (file != NULL) { fclose(file); } }

void skip_empty(FILE *file)
{
    int c;
    while ((c = fgetc(file)) != EOF) 
    {
       char character = (char)c;
       if (character != '\n' && character != ' ' && character != '\t')  { ungetc(c, file); break; }
    }
}

char * get_lexema(FILE *file, int *min_base, enum input_statements *stm)
{
    char *lexema = (char *)malloc(MAX_LENGTH * sizeof(char));
    if (lexema == NULL) {*stm = input_invalid; return NULL;}

    int c; *min_base = 0; 
    bool flag = true;
    *stm = input_correct;

    size_t originalLength = 0;

    while ((c = fgetc(file)) != EOF) 
    {
        char character = (char)c;
        if (character == '\n' || character == ' ' || character == '\t') { break; }
        
        if (originalLength < MAX_LENGTH) 
        {
            int b = find_min_base(character);
            if (b == -1) {flag = false;}
            if (*min_base < b) { *min_base = b; }

            lexema[originalLength] = character;
            originalLength++;
        } 
        else{ *stm = input_invalid; return NULL; }
    }
    if (!flag) {*stm = input_invalid; return NULL;}

    lexema[originalLength] = '\0';
    if (*min_base == 1) {*min_base = 2;}
    return lexema;
}

enum input_statements cycle(FILE *input, FILE* output)
{
    int c;
    bool first_in_que = true;
    int lexema_cnt = 1;

    while (!feof(input))
    { 
        skip_empty(input);
        enum input_statements stm;
        int base;
        
        char *lexema = get_lexema(input, &base, &stm);

        if (stm != input_correct) { free(lexema); continue; }
        
        long long number = 0;
        stm = ss_to_base_10(lexema, base, &number);

        if (stm != input_correct) { free(lexema); continue; }

        fprintf(output, "<<< \"%s\" with min base <%d> in <10> CC is : \"%lld\"\n", lexema, base, number);
        
        lexema_cnt++;
        free(lexema);
    }
   
    return input_correct;
}