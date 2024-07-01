#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STR_BUF 1024

typedef struct
{
    int id;
    char name[STR_BUF];
    char sur_name[STR_BUF];
    double money;
} Employee;


typedef enum
{
    correct,
    runtime_error,
    invalid_input,
    memory_error,
    invalid_file,
    end_of,
} statements;


void print_err(statements stm){
    printf("Error : occured error with number : %d\n", stm);
}

int comp_right(const void *a, const void *b) {
    Employee *first = (Employee *)a, *second =(Employee *)b;
    int res;

    res = first->money - second->money;
    if (res != 0) {return res; }

    res = strcmp(first->sur_name, second->sur_name);
    if (res != 0) {return res; }

    res = strcmp(first->name, second->name);
    if (res != 0) {return res; }

    res = first->id - second->id;
    return res; 
}


int comp_left(const void *a, const void *b)
{
    return comp_right(b, a); 
}


statements sort_by_increase(Employee **employers, int cnt)
{
    if (cnt < 1) {return invalid_input;}
    qsort(*employers, cnt, sizeof((*employers)[0]), comp_right);
    return correct;
}

statements sort_by_decrease(Employee **employers, int cnt)
{
    if (cnt < 1) {return invalid_input;}
    qsort(*employers, cnt, sizeof((*employers)[0]), comp_left);
    return correct;
}

void print_employers(Employee *empls, int cnt)
{
    printf("\n");
    for (int i = 0; i < cnt; i++)
    {
        printf("Salary: %.2lf, Surname: %s,  Name: %s, ID: %d\n",
               empls[i].money, empls[i].sur_name, empls[i].name, empls[i].id);
    }
    printf("\n");
}

statements validate_params(int argc, char *argv[], FILE** file, char *flag)
{
    if (argc < 3 || strlen(argv[2]) != 2) {return invalid_input; }
    char a = argv[2][0], b = argv[2][1];
    if (a != '/' && a != '-') {return invalid_input; }
    if (b != 'd' && b != 'a') {return invalid_input; }
    *flag = b;
    *file = fopen(argv[1], "r");
    
    if (*file == NULL)
    {
        return invalid_file;
    }
    return correct;
}

statements scan_eployee(FILE *file, Employee *emp)
{
    int id;
    char name[STR_BUF];
    char sur_name[STR_BUF];
    double money;

    if(fscanf(file, "%d %s %s %lf\n", &id, name, sur_name, &money) != 4)
    {
        return end_of;
    }
    emp->id = id;
    strcpy(emp->name, name);
    emp->money =  money;
    strcpy(emp->sur_name, sur_name);

    //printf("Cотрудник %d %s %s %lf\n", id, name, sur_name, money);
    return correct;
}