#include "Student.c"

Student **update_data(FILE *file, int *n)
{
    statements stm;
    size_t buf = 32, realn = 0;

    Student **mas = (Student **)malloc(sizeof(Student *) * buf);
    if (mas == NULL)
    {
        return NULL;
    }
    Student *curr;
    while ((curr = read_student(file, &stm)) != NULL)
    {
        fprintf(stdout, "Name is %s, Surname is %s, %s, %d\n", curr->name->value, curr->s_name->value, curr->group->value, curr->mas[4]);
        if (buf > realn + 1)
        {
            buf *= 2;
            Student **temp = (Student **)realloc(mas, sizeof(Student *) * buf);
            if (temp == NULL)
            {
                free(mas);
                return NULL;
            }
            mas = temp;
        }
        mas[realn++] = curr;
    }
    Student **temp = (Student **)realloc(mas, sizeof(Student *) * realn);
    if (temp == NULL)
    {
        free(mas);
        return NULL;
    }
    mas = temp;

    *n = realn;
    return mas;
}

void skippy_empty()
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

void print_usage()
{
    printf(">>> <s> Sort students by ..\n");
    printf(">>> <f> Find students by ..\n");
    printf(">>> <t> Print good students\n");
    printf(">>> <q> Exit from menu\n\n\n");
}

statements run(char *path)
{
    FILE *data = NULL;
    data = fopen(path, "r");
    if (data == NULL)
    {
        return invalid_file;
    }
    int n;
    Student **students = update_data(data, &n);
    fclose(data);

    if (students == NULL)
    {
        return runtime_error;
    }

    print_usage();

    while (true)
    {
        // skippy_empty();
        printf("-: ");
        char input;
        scanf("%c", &input);

        skippy_empty();
        if (input == 's')
        {
            printf("Select sort parameter\n");
            printf(">>> <i> by id\n");
            printf(">>> <g> by group\n");
            printf(">>> <n> by name\n");
            printf(">>> <s> by surname\n\n");
            printf("-: ");
            fflush(stdin);
            scanf("%c", &input);
            
            skippy_empty();
            CompareFunction ptr = NULL;
            if (input == 'i')
            {
                ptr = compare_by_id;
            }
            if (input == 'g')
            {
                ptr = compare_by_group;
            }
            if (input == 'n')
            {
                ptr = compare_by_name;
            }
            if (input == 's')
            {
                ptr = compare_by_sname;
            }
            if (ptr != NULL)
            {
                sort_students(students, n, ptr);
                printf("Sorted succesfully\n");
            }
        }
        if (input == 't')
        {
            printf("Enter out file path\n");
            printf("-: ");

            char path[1024] = "";

            if (scanf("%s", path) != 1)
            {
                printf("Something wrong of Not founded\n");
                continue;
            }
            FILE *out = NULL;

            out = fopen(path, "w");

            if (out == NULL)
            {
                printf("Invalid file\n");
                continue;
            }
            int new_cnt = n;
            Student **good = get_better_mid_students(students, &new_cnt);
            if (good == NULL)
            {
                fclose(out);
                printf("Something wrong of Not founded\n");
                continue;
            }
            printf("%d\n", good[0]->id);
            for (int i = 0; i < new_cnt; i++)
            {
                print_student(good[i], out);
            }

            fclose(out);
            free(good);
        }
        if (input == 'f')
        {
            printf(">>> <i> by id\n");
            printf(">>> <g> by group\n");
            printf(">>> <n> by name\n");
            printf(">>> <s> by surname\n\n");
            printf("Input the searching parameter\n-: ");
            // skippy_empty();
            scanf("%c", &input);
            // skippy_empty();

            if (input == 'i')
            {
                int ids;
                printf("Input the searching value\n-: ");
                if (scanf("%d", &ids) != 1)
                {
                    printf("Error\n");
                    continue;
                }

                Student *res = find_student_by_id(students, n, ids);
                if (res == NULL)
                {
                    printf("Error or None\n");
                    continue;
                }
                printf("Enter out file path\n");
                printf("-: ");

                char path[1024] = "";

                if (scanf("%s", path) != 1)
                {
                    printf("Something wrong of Not founded\n");
                    continue;
                }
                FILE *out = NULL;

                out = fopen(path, "w");

                if (out == NULL)
                {
                    printf("Invalid file\n");
                    continue;
                }
                trace_student(res, out);
                fclose(out);
                printf("Success");
                continue;
            }
            Student **res;
            char line[1024];

            printf("Input the searching value\n-: ");

            if (scanf("%s", line) != 1)
            {
                printf("Error\n");
                continue;
            }
            if (input == 'g')
            {
                res = find_students_by_group(students, n, line);
            }
            else if (input == 'n')
            {
                res = find_students_by_name(students, n, line);
            }
            else if (input == 's')
            {
                res = find_students_by_sname(students, n, line);
            }
            else
            {
                printf("Error: <%c> not valid param\n", input);
                continue;
            }

            if (res == NULL)
            {
                printf("Error or None\n");
                continue;
            }
            int reals = 0;
            Student *curr = res[reals];
            while (curr != NULL)
            {
                trace_student(curr, stdout);
                curr = res[reals++];
            }
            free(res);
            printf("\n");
        }
        if (input == 'q')
        {
            break;
        }
    }

    for (int i = 0; i < n; i++)
    {
        delete_student(students[i]);
    }
    free(students);
}