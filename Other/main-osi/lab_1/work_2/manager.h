#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

enum statements
{
    invalid_login,
    invalid_password,
    invalid_flag,
    memory_err,
    
    correct,

};

void print_hint()
{
    printf("<< Please register to get access to commands\n");
    printf("<< len(login) < 7 and 0 < password < 100000 \n");
    printf("<< Awaiting : login and password : ");
}

void print_hint2()
{
    printf("<< Please write you wanna to register or login? \n");
    printf("<< Awaiting : \'Login\' or \'Register\' : ");
}

bool clear_flow()
{
    int c; bool cleared = false;
    while ((c = getchar()) != '\n' && c != EOF) {cleared = true;};
    return cleared;
}

char *strdubl(const char *src)
{
    char *dst = malloc(strlen (src) + 1);
    if (dst == NULL) return NULL;
    strcpy_s(dst, strlen (src) + 1,  src); 
    return dst;   
}

char** split_string(const char* input, int* token_count) 
{
    char** tokens = NULL;
    char *context = NULL;

    char* token = strtok_s((char*)input, " \t\n", &context);
    
    *token_count = 0;

    while (token != NULL) {
        char** new_tokens = realloc(tokens, (*token_count + 1) * sizeof(char*));

        if (new_tokens == NULL)
        {
            printf("Error : don't have enough memory \n");
            if (tokens == NULL) { return NULL; }
            for (int i = 0; i < *token_count; i++) { free(tokens[i]); }
            free(tokens);
            return NULL;
        }

        tokens = new_tokens;
        tokens[(*token_count)] = strdubl(token);

        (*token_count)++;
        if (tokens[(*token_count)-1] == NULL)
        {
            printf("Error : can't copy string \n");
            if (tokens == NULL) { return NULL; }
            for (int i = 0; i < *token_count; i++) { free(tokens[i]); }
            free(tokens);
            return NULL;
        }

        token = strtok_s(NULL, " \t\n", &context);
    }
    return tokens;
}

char ** get_string(int *tokens_cnt)
{
    char input_buffer[100];
    bool newline_found = false; 

    for (int i = 0; i < 99; i++) 
    {
        int c = getchar();
        if (c == EOF) { break; }

        if (c == '\n') { newline_found = true;  break; }

        input_buffer[i] = c;
        input_buffer[i + 1] = '\0';
    }
    if (!newline_found) {
        bool cleared = clear_flow();
    }
    return split_string(input_buffer, tokens_cnt);

}

char** retister_or_login()
{
    print_hint2();
    int l;
    char ** input = get_string(&l);
    printf("\n");
    if (l != 1) { free(input); return NULL; }
    if (strcmp(input[0], "Login") != 0 && strcmp(input[0], "Register") != 0 && strcmp(input[0], "Quit") != 0  ) { free(input); return NULL; }
    return input;
}

long long char_to_ll(char * str)
{
    char* endptr;
    errno = 0;

    long long number = strtol(str, &endptr, 10);

    if ((errno == ERANGE && (number == LLONG_MAX || number == LLONG_MIN)) ||
        (errno != 0 && number == 0)) 
    {
        printf("Error : can't transform\n");
        return -1;
    }

    if (*endptr != '\0') {
        printf("Error : can't transform all string\n");
        return -1;
    }
    return number;

}

int get_int(char *str)
{
    long long number = char_to_ll(str);
    if (number != -1 && number < INT_MAX && number > INT_MIN){
        return (int)number;
    }
    return -1;
}

 struct tm* get_time(char* input)
 {
    struct tm* result = (struct tm*) malloc(sizeof(struct tm));
    if (result == NULL) {return NULL;}
    struct tm date;

    if (sscanf_s(input, "%d-%d-%d", &date.tm_mday, &date.tm_mon, &date.tm_year) != 3) { free(result); return NULL; }
   
    if (date.tm_year < 1900 || date.tm_year > 2500) 
    {
        printf("Incorrect year\n");
        return NULL;
    } else if (date.tm_mon < 1 || date.tm_mon > 12) 
    {
        printf("Incorrect month\n");
        return NULL;
    }
    date.tm_year -= 1900;
    date.tm_mon--;

    date.tm_hour = 0;
    date.tm_min = 0;
    date.tm_sec = 0;
    *result = date;

    return result;
}

enum statements get_time_format(char *string, int *format)
{
    *format = -1;
    if (string == NULL) {return invalid_flag; }
    if (strlen(string) != 2) { return invalid_flag;}

    if (string[0] != '-' && string[1] != '/') { return invalid_flag; }
    if (string[1] != 's' && string[1] != 'm' && string[1] != 'h' && string[1] != 'y')  { return invalid_flag; }
    switch (string[1])
    {
        case 's':
            *format = 1;
            break;
        case 'm':
            *format = 60;
            break;
        case 'h':
            *format = 3600;
            break;
        case 'y':
            *format = 365 * 24 * 3600;
            break;
        default:
            break;
    }
    return correct;
}

void free_mas(char** mas, int l){
    for (int i = 0; i < l; i++)
    {
        free(mas[i]);
    }
    free(mas);
}

