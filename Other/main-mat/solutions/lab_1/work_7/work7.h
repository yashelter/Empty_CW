#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


#define MAX_STRING_SIZE 100


//moves pointer to first non empty
void skip_empty(FILE *file)
{
    int c;
    while ((c = fgetc(file)) != EOF) 
    {
       char character = (char)c;
       if (character != '\n' && character != ' ' && character != '\t')  { ungetc(c, file); break; }
    }
}

bool is_latin_letter(char symbol) 
{
    return isalpha(symbol) && ((symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z'));
}


void lexema_to_lower(char* str)
{
    for (int i = 0; i < strlen(str); i++) 
    {
        if(is_latin_letter(str[i])) { str[i] = tolower(str[i]); }
    }
}

enum base_statements
{
    base_too_long_string,
    base_correct,
    base_invalid_input,
    base_error_in_function
};


enum r_statements
{
    r_file_error,
    r_correct,
    r_invalid_base_statement
};


enum a_statements
{
    a_num_overflow,
    a_num_underflow,
    a_correct,
    a_file_error,
    a_invalid_base_statement
};

// при перевыделении памяти нужно исп времен переменную, в неё результат realloc проверить на null 
// если null очистить память

char* get_lexema(FILE *file, enum base_statements *ex_code)
{
    char *lexema = (char *)malloc(MAX_STRING_SIZE * sizeof(char));
    if (lexema == NULL) {return "";}//
    int c;
    size_t originalLength = 0;
    *ex_code = base_correct;

    while ((c = fgetc(file)) != EOF) 
    {
        char character = (char)c;
        if (character == '\n' || character == ' ' || character == '\t') { break; }
        
        if (originalLength < MAX_STRING_SIZE) 
        {
            lexema[originalLength] = character;
            originalLength++;
        }
        else{ *ex_code = base_too_long_string; return NULL; }
    }
    lexema[originalLength] = '\0';
    return lexema;
}



enum base_statements read_and_write_r(FILE *file1, FILE *file2, FILE *result)
{
    int c;
    bool first_in_que = true;
    skip_empty(file1);
    skip_empty(file2);   
    
    while (!(feof(file1) && feof(file2))) 
    {
        enum base_statements stm;
        if (first_in_que)
        {
            if (feof(file1)) { first_in_que = false; continue; }
            
            char *lexema = get_lexema(file1, &stm);
            if (stm != base_correct) {return stm; }
            fprintf(result, "%s ", lexema);

            free(lexema);
            skip_empty(file1);
            first_in_que = false;
        
        }
        else
        {
            if (feof(file2)) { first_in_que = true; continue; }
            
            char *lexema = get_lexema(file2, &stm);
            if (stm != base_correct) {return stm; }
            fprintf(result, "%s ", lexema);

            free(lexema);
            skip_empty(file2);
            first_in_que = true;
        }
    }
    return base_correct;

}

enum r_statements flag_r(char *path_file1, char *path_file2, char *path_res)
{
    FILE *file1, *file2, *result;
    errno_t err;

    err = fopen_s(&file1, path_file1, "r");
    if (err != 0) { return r_file_error; }

    err = fopen_s(&file2, path_file2, "r");
    if (err != 0) { return r_file_error; }

    err = fopen_s(&result, path_res, "w");
    if (err != 0) { return r_file_error; }

    enum base_statements stm = read_and_write_r(file1, file2, result);
    
    fclose(file1);
    fclose(file2);
    fclose(result);

    if (stm != base_correct) {return r_invalid_base_statement; }

    return r_correct;
}


enum base_statements write_to_file_in_cc(FILE *file, int num, const int CC)
{
    char converted_num[MAX_STRING_SIZE];
    int index = 0;
    while (num > 0)
    {
        int remainder = num % CC;
        converted_num[index] = '0' + remainder;
        index++;
        num /= CC;
        if (index + 1 > MAX_STRING_SIZE){ return base_too_long_string; }
    }
    if (index == 0) 
    {
        converted_num[index] = '0';
        index++;
    }
    converted_num[index] = '\0';

    for (int i = index-1; i >= 0; i--) 
    {
        char a =  '0' + i;
        fprintf(file, "%c", converted_num[i]);
    }
    return base_correct;
}




enum a_statements to_ascii_and_cc(FILE *file, char* lexema, int CC)
{
    char *endptr;
    for (int i = 0; i < strlen(lexema); i++) 
    {
        
        int num = (int) lexema[i];

         // debug option:  printf("%c %d\n",lexema[i],  num);
        if (num < 0) { return a_num_underflow; }
        enum base_statements stm = write_to_file_in_cc(file, num, CC);
        if (stm != base_correct) { return a_invalid_base_statement; }
		
        else { printf("%c", lexema[i]); }
    }
    return a_correct;
}

enum a_statements write_to_file_a(FILE *file, FILE *result)
{
    int c;
    bool first_in_que = true;
    skip_empty(file);
    int lexema_cnt = 1;

    while (!feof(file))
    {
        enum base_statements stm;
        char *lexema = get_lexema(file, &stm);

        if (lexema_cnt % 10 == 0)
        {
            lexema_to_lower(lexema);
            enum a_statements state  = to_ascii_and_cc(result, lexema, 4);
            if (state != a_correct) { return state;}
            fprintf(result, " ");
        }
        else if (lexema_cnt % 2 == 0)
        {
            char *endptr;
            lexema_to_lower(lexema);
            fprintf(result, "%s ", lexema);
            
        }
        else if (lexema_cnt % 5 == 0)
        {
            lexema_to_lower(lexema);
            enum a_statements state  = to_ascii_and_cc(result, lexema, 8);
            if (state != a_correct) { return state;}
            fprintf(result, " ");
        }
        else { fprintf(result, "%s ", lexema); }
        lexema_cnt++;
        free(lexema);
        skip_empty(file);
        
    }
    return a_correct;
}


enum a_statements flag_a(char *path1, char *path2)
{
    FILE *file1, *result;
    errno_t err;

    err = fopen_s(&file1,  path1, "r");
    if (err != 0) {  return a_file_error;}

    err = fopen_s(&result, path2, "w");
    if (err != 0) { return a_file_error; }

    enum a_statements stm =  write_to_file_a(file1, result);

    fclose(file1);
    fclose(result);

    return stm;
}

enum base_statements check_flag(int argc, char* argv[], char *flag)
{
    if (argc < 2) {return base_invalid_input;}
    if (strlen(argv[1]) != 2 || (argv[1][0] != '-' && argv[1][0] != '/')) {return base_invalid_input; }
    if (argv[1][1] != 'r' && argv[1][1] != 'a') { return base_invalid_input; }
    *flag = argv[1][1];
    return base_correct;
}

enum base_statements check_input_validate_n_run(int argc, char* argv[], char *flag)
{
    enum base_statements flag_checkpoint = check_flag(argc, argv, flag);
     
    if (flag_checkpoint != base_correct) {return base_invalid_input;}
    
    switch (*flag)
    {
    case 'r':
        if (argc != 5)
        {
            printf("Error: Check input paramers, something wrong\n");
            return base_invalid_input;
        }
        
        enum r_statements stm1 = flag_r(argv[2], argv[3], argv[4]);
        if (stm1 != r_correct) {return base_error_in_function; }

        return base_correct;
        break;
    case 'a':
        if (argc != 4)
        {
            printf("Error: Check input paramers, something wrong\n");
            return base_invalid_input;
        }
        enum a_statements stm2 = flag_a(argv[2], argv[3]);
        if (stm2 != a_correct) {return base_error_in_function; }

        return base_correct;
        break;

    default:
        break;
    }

    return base_invalid_input;
    
}