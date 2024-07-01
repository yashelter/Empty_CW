/*
Через аргументы
командной строки программе на вход подаётся путь к файлу, содержащему записи о
студентах. При старте программа считывает поданный файл в динамический массив
структур типа Student. В программе должен быть реализован поиск всех студентов по
*/

#include "../4/String.c"
typedef int (*CompareFunction)(const void *, const void *);

typedef struct
{
    unsigned int id;

    String *name;
    String *s_name;
    String *group;

    unsigned char *mas;
    int m_size;
} Student;

void delete_student(Student *st)
{
    delete_string(st->name);
    delete_string(st->s_name);
    delete_string(st->group);
    free(st->mas);
    free(st);
}
Student *create_student(int id, String *name, String *s_name, String *group, unsigned char *mas, int l, statements *stm)
{
    Student *st = (Student *)malloc(sizeof(Student));
    if (st == NULL)
    {
        *stm = allocate_error;
        return NULL;
    }
    st->id = id;
    st->name = name;
    st->s_name = s_name;
    st->group = group;
    st->mas = mas;
    st->m_size = l;
    return st;
}

Student *create_student_s(int id, char *name, char *s_name, char *group, unsigned char *mas, int l, statements *stm)
{
    String *_name, *_s_name, *_group;
    *stm = create_string(name, &_name);
    if (*stm != correct)
    {
        return NULL;
    }

    *stm = create_string(s_name, &_s_name);
    if (*stm != correct)
    {
        delete_string(_name);
        return NULL;
    }

    *stm = create_string(group, &_group);
    if (*stm != correct)
    {
        delete_string(_name);
        delete_string(_s_name);
        return NULL;
    }
    return create_student(id, _name, _s_name, _group, mas, l, stm);
}

int compare_by_id(const void *a, const void *b)
{
    const Student *_a = (const Student *)a;
    const Student *_b = (const Student *)b;

    return _a->id - _b->id;
}

int compare_by_name(const void *a, const void *b)
{
    const Student *_a = (const Student *)a;
    const Student *_b = (const Student *)b;

    return compare_strings(_a->name, _b->name);
}

int compare_by_sname(const void *a, const void *b)
{
    const Student *_a = (const Student *)a;
    const Student *_b = (const Student *)b;

    return compare_strings(_a->s_name, _b->s_name);
}

int compare_by_group(const void *a, const void *b)
{
    const Student *_a = (const Student *)a;
    const Student *_b = (const Student *)b;

    return compare_strings(_a->group, _b->group);
}

Student *find_student_by_id(Student **stds, int n, int id)
{
    for (int i = 0; i < n; i++)
    {
        Student *s = stds[i];
        if (s->id == id)
        {
            return s;
        }
    }
    return NULL;
}

Student **find_students_by_name(Student **stds, int n, char * name)
{
    Student ** res = (Student **) malloc(sizeof(Student *) * n);
    int real = 0;
    if (res == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < n; i++)
    {
        Student *s = stds[i];
        if (strcmp(s->name->value, name) == 0)
        {
            res[real++] = s;
        }
    }
    Student ** temp = (Student **) realloc(res, sizeof(Student *) * (real + 1));
    if (temp == NULL){
        free(res);
        return NULL;
    }
    res = temp;
    res[real] = NULL;
    return res;
}
Student **find_students_by_sname(Student **stds, int n, char * sname)
{
    Student ** res = (Student **) malloc(sizeof(Student *) * n);
    int real = 0;
    if (res == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < n; i++)
    {
        Student *s = stds[i];
        if (strcmp(s->s_name->value, sname) == 0)
        {
            res[real++] = s;
        }
    }
    Student ** temp = (Student **) realloc(res, sizeof(Student *) * (real + 1));
    if (temp == NULL){
        free(res);
        return NULL;
    }
    res = temp;
    res[real] = NULL;
    return res;
}

Student **find_students_by_group(Student **stds, int n, char * group)
{
    Student ** res = (Student **) malloc(sizeof(Student *) * n);
    int real = 0;
    if (res == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < n; i++)
    {
        Student *s = stds[i];
        if (strcmp(s->group->value, group) == 0)
        {
            res[real++] = s;
        }
    }
    Student ** temp = (Student **) realloc(res, sizeof(Student *) * (real + 1));
    if (temp == NULL){
        free(res);
        return NULL;
    }
    res = temp;
    res[real] = NULL;
    return res;
}


void sort_students(Student **stds, int size, CompareFunction func)
{
    qsort(stds, size, sizeof(Student *), func);
}

double calulate_mid(Student **stds, int size)
{
    double res = 0;
    for (int i = 0; i < size; i++)
    {
        res = res + (sum_d((stds[i])->mas, (stds[i])->m_size));
    }
    return res / size;
}

String *get_student_info(Student *sd)
{
    String *s;
    statements stm;
    stm = create_string("SN is : ", &s);
    if (stm != correct)
    {
        return NULL;
    }

    stm = concat_string(s, sd->name);
    if (stm != correct)
    {
        return NULL;
    }

    stm = string_add_line(s, " ");
    if (stm != correct)
    {
        return NULL;
    }

    stm = concat_string(s, sd->s_name);
    if (stm != correct)
    {
        return NULL;
    }
    stm = string_add_line(s, " , group is : ");
    if (stm != correct)
    {
        return NULL;
    }
    stm = concat_string(s, sd->group);
    if (stm != correct)
    {
        return NULL;
    }

    stm = string_add_line(s, " , mid mark is : ");
    if (stm != correct)
    {
        return NULL;
    }

    char line[20];
    double mark = sum_d(sd->mas, sd->m_size);
    snprintf(line, 20, "%f\n", mark);

    stm = string_add_line(s, line);
    if (stm != correct)
    {
        return NULL;
    }
    return s;
}

Student **get_better_mid_students(Student **stds, int *cnt)
{
    int n = *cnt;
    Student **result = (Student **)malloc(sizeof(Student *) * n);
    if (result == NULL)
    {
        return NULL;
    }

    double bs = calulate_mid(stds, n);
    int counter = 0;
    for (int i = 0; i < n; i++)
    {
        if (sum_d(stds[i]->mas, stds[i]->m_size) > bs)
        {
            result[counter++] = stds[i];
        }
    }
    Student **temp = (Student **)realloc(result, sizeof(Student *) * (counter + 1));
    if (temp == NULL)
    {
        free(result);
        return NULL;
    }
    result = temp;
    *cnt = counter;
    return result;
}
bool print_student(Student *st, FILE *file)
{
    fprintf(file, "Name is %s, Surname is %s\n", st->name->value, st->s_name->value);
    return true;
}
void trace_student(Student *st, FILE *file){
    fprintf(file, "Name is %s, Surname is %s, group is %s, id=%u, mid n=%lf\n", st->name->value, st->s_name->value, st->group->value, 
    st->id, sum_d(st->mas, st->m_size)); 
}
bool print_students(Student **stds, int n, FILE *file)
{
    if (file == NULL)
    {
        return false;
    }
    for (int i = 0; i < n; i++)
    {
        bool c = print_student(stds[i], file);
        if (!c)
        {
            return c;
        }
    }
    return true;
}

statements trace_students(Student **stds, int n, const char *path)
{
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        return invalid_file;
    }
    print_students(stds, n, file);
    fclose(file);
    return correct;
}

Student *read_student(FILE *file, statements *state)
{
    unsigned int id;
    String *name;
    String *s_name;
    String *group;
    *state = runtime_error;

    unsigned char *mas = (unsigned char *)malloc(sizeof(unsigned char) * 5);
    if (mas == NULL)
    {
        return NULL;
    }

    if (fscanf(file, "%u", &id) != 1)
    {
        free(mas);
        return NULL;
    }

    char ch = ' ';
    skip_empty(file, ' ', &ch);

    name = input_string(file, state);
    if (*state != correct)
    {
        free(mas);
        return NULL;
    }

    s_name = input_string(file, state);
    if (*state != correct)
    {
        free(mas);
        delete_string(name);
        return NULL;
    }

    group = input_string(file, state);

    if (*state != correct)
    {
        free(mas);
        delete_string(name);
        delete_string(s_name);
        return NULL;
    }
    int n = fscanf(file, "%hhu %hhu %hhu %hhu %hhu", &mas[0], &mas[1], &mas[2], &mas[3], &mas[4]);

    ch = 'a';
    skip_empty(file, '\n', &ch);

    if (n != 5)
    {
        free(mas);
        delete_string(name);
        delete_string(s_name);
        delete_string(group);
        return NULL;
    }

    for (int i = 0; i < 5; i++)
    {
        if (mas[i] < 0 || mas[i] > 5)
        {
            free(mas);
            delete_string(name);
            delete_string(s_name);
            delete_string(group);
            *state = invalid_input;
            return NULL;
        }
    }

    Student *result = create_student(id, name, s_name, group, mas, 5, state);

    if (*state != correct)
    {
        free(mas);
        delete_string(name);
        delete_string(s_name);
        delete_string(group);
        return NULL;
    }
    *state = correct;
    print_student(result, stdout);

    return result;
}
