#include "funcs.c"

#define TEMP "temp.txt"
#define MAX_L 1024
#define OPERATABLE 64


char **split_perc(char*str, int *point)
{
    *point = -1;
    int l = strlen(str);
    int i = 0;

    for (; i < l - 1; i++) 
    { 
        if (str[i] == '%' && str[i+1] != '%'){break;}
        else if (str[i] == '%' ) { i++;}
    }

    if (str[i] == '%' && str[i+1] != ' ') {*point = i;}
    else
    {
        return NULL;
    }

    int pos_end = i+1;
    while (pos_end < l && str[pos_end] != ' ' && str[pos_end] != '%' && str[pos_end] != '\n' && str[pos_end] != '\t'&& !not_digit(str[pos_end])) {pos_end++;};

    //here
    char *s1 = (char*) malloc(sizeof(char) * (i + 1));
    if (s1 == NULL) {return NULL;}

    char *s2 = (char*) malloc(sizeof(char) * (pos_end - i + 1));
    if (s2 == NULL) {free(s1); return NULL;}

    char *s3 = (char*) malloc(sizeof(char) * (l - pos_end + 1));
    if (s3 == NULL) {free(s1); free(s2); return NULL;}

    for (int j = 0; j < i; j++) 
    {
        s1[j] = str[j];
    }
    s1[i] = '\0';

    int k = 0;
    for (int j = i; j < pos_end; j++) {
        s2[k] = str[j];
        k++;
    }
    s2[k] = '\0'; // Завершите вторую подстроку нулевым символом

    // Выделение третьей подстроки (после пробела)
    int m = 0;
    for (int j = pos_end; j < l; j++) {
        s3[m] = str[j];
        m++;
    }
    s3[m] = '\0'; // Завершите третью подстроку нулевым символом

    // Выделяем память для массива указателей
    char **res = (char**) malloc(sizeof(char *) * 3);
    if (res == NULL) {
        free(s1);
        free(s2);
        free(s3);
        return NULL;
    }

    res[0] = s1;
    res[1] = s2;
    res[2] = s3;

    return res;

}

statements try_executef(char *format, va_list *ptr, FILE* stream)
{
    if (strcmp(format, "%Ro") == 0)
    {
        int* num = va_arg(*ptr, int *);
        char roman_str[OPERATABLE];

        if (fscanf(stream, "%s", roman_str) != 1) { return -1; }
        statements stm = from_roman(roman_str, &num);
        return stm;
    }
    if (strcmp(format, "%Zr") == 0)
    {
        unsigned int* num = va_arg(*ptr, unsigned int*);

        char zeckendorf_str[OPERATABLE];
        if (fscanf(stream, "%s", zeckendorf_str) != 1) {  return -1; }

        statements stm = zeckendorf_look_int(zeckendorf_str, &num);
        return stm;
        
    }
    if (strcmp(format, "%Cv") == 0)
    {
        int* num = va_arg(*ptr, int*);
        int base = va_arg(*ptr, int);

        char base_str[OPERATABLE];
        if (fscanf(stream, "%s", base_str) != 1) { return -1; }

        statements stm = ss_to_base_10(base_str, base, &num, false);
        return stm;
    }
    if (strcmp(format, "%CV") == 0)
    {
        
        int* num = va_arg(*ptr, int*);
        int base = va_arg(*ptr, int);

        char base_str[OPERATABLE];
        if (fscanf(stream, "%s", base_str) != 1) { return -1; }

        statements stm = ss_to_base_10(base_str, base, &num, true);
        return stm;
    }
    return nah_executed;
    
}

void move(char *format, va_list *ptr){
     if (strcmp(format, "%d") == 0)
    {
        int* num = va_arg(*ptr, int*);
    }
    else if (strcmp(format, "%c") == 0)
    {
        char* num = va_arg(*ptr, char*);
    }
    else if (strcmp(format, "%f") == 0)
    {
        float* num = va_arg(*ptr, float*);
    }
    else if (strcmp(format, "%d") == 0)
    {
        double* num = va_arg(*ptr, double*);
    }
    else if (strcmp(format, "%s") == 0)
    {
        char** num = va_arg(*ptr, char**);
    }
    else if (strcmp(format, "%флажок") == 0)
    {
        //...//
    }
}

int overfscanf(FILE* file, char* format, ...)
{
    va_list ptr;
    va_start(ptr, format);

    int pos;

    char* right = NULL;
    char ** res = split_perc(format, &pos);
    
    while (res != NULL)
    {
        //printf("%p\n", ptr);
        char *left = res[0], *mid = res[1];
        right = res[2];
        
        char *result = NULL;
        vfscanf(file, left, ptr);
        
        if (mid != NULL && strlen(mid) > 0) 
        {
            statements scanned = try_executef(mid, &ptr, file);
            
            if (scanned == nah_executed)
            {
                vfscanf(file, mid, ptr);
                move(mid, &ptr);
            }          
        }
        if (res[0] != NULL) {free(res[0]); res[0] = NULL;}
        if (res[1] != NULL) {free(res[1]); res[1] = NULL;}
        
        if (strlen(right) > 0)
        {
            char **new_res = split_perc(right, &pos);
            if (new_res == NULL)
            {
                vfscanf(file, right, ptr);
            }
            if (res[2] != NULL) {free(res[2]); res[2] = NULL;}
            free(res);

            res = NULL;
            right = NULL;

            res = new_res;
        }
        else
        {
            if (right != NULL) {free(res[2]);res[2] = NULL;}
            break;
        }
    }
    
    if (res != NULL) 
    { 
        if (res[0] != NULL) {free(res[0]);}
        if (res[1] != NULL) {free(res[1]);}
        if (res[2] != NULL) {free(res[2]);}
        free(res); 
    }
    
    va_end(ptr);
    return 0;
}

int oversscanf(char *string, char* format, ...)
{
    FILE *file = fopen(TEMP, "w");
    if (file == NULL)
    {
        return -1;
    }

    fprintf(file, "%s", string);
    fclose(file);

    file = fopen(TEMP, "r");
    if (file == NULL)
    {
        return -1;
    }

    va_list ptr;
    va_start(ptr, format);

    int pos;

    char* right = NULL;
    char ** res = split_perc(format, &pos);
    
    while (res != NULL)
    {
        //printf("%p\n", ptr);
        char *left = res[0], *mid = res[1];
        right = res[2];
        
        char *result = NULL;
        vfscanf(file, left, ptr);
        
        if (mid != NULL && strlen(mid) > 0) 
        {
            statements scanned = try_executef(mid, &ptr, file);
            
            if (scanned == nah_executed)
            {
                vfscanf(file, mid, ptr);
                move(mid, &ptr);
            }          
        }
        if (res[0] != NULL) {free(res[0]); res[0] = NULL;}
        if (res[1] != NULL) {free(res[1]); res[1] = NULL;}
        
        if (strlen(right) > 0)
        {
            char **new_res = split_perc(right, &pos);
            if (new_res == NULL)
            {
                vfscanf(file, right, ptr);
            }
            if (res[2] != NULL) {free(res[2]); res[2] = NULL;}
            free(res);

            res = NULL;
            right = NULL;

            res = new_res;
        }
        else
        {
            if (right != NULL) {free(res[2]);res[2] = NULL;}
            break;
        }
    }
    
    if (res != NULL) 
    { 
        if (res[0] != NULL) {free(res[0]);}
        if (res[1] != NULL) {free(res[1]);}
        if (res[2] != NULL) {free(res[2]);}
        free(res); 
    }
    
    va_end(ptr);
    fclose(file);
    
    if (remove(TEMP) == 0) 
    {
        return 0;
    } else {
        return 2;
    }
    
}