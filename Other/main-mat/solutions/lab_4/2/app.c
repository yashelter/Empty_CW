#include "Array.c"
#define SYMBS 26


int compare_asc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int compare_des(const void *a, const void *b)
{
    return (*(int *)b - *(int *)a);
}

statements sort_array(Array *arr, char order)
{
    if (arr->length == 0)
        return invalid_array;

    int (*compare_function)(const void *, const void *);
    if (order == '+')
    {
        compare_function = compare_asc;
    }
    else if (order == '-')
    {
        compare_function = compare_des;
    }
    else
    {
        return invalid_command;
    }

    qsort(arr->data, arr->length, sizeof(int), compare_function);

    return correct;
}

int random_compare(const void *left, const void *right)
{
    int val = (rand() % INT_MAX) - (INT_MAX / 2);
    return val;
}

statements shuffle_array_s(Array *arr)
{
    if (arr->length == 0)
    {
        return invalid_array;
    }
    qsort(arr->data, arr->length, sizeof(int), random_compare);
}

statements find_max(const Array *arr, int *max_value, int *max_index)
{
    if (arr->length == 0)
        return invalid_array;

    *max_value = arr->data[0];
    *max_index = 0;

    for (int i = 1; i < arr->length; ++i)
    {
        if (arr->data[i] > *max_value)
        {
            *max_value = arr->data[i];
            *max_index = i;
        }
    }
    return correct;
}

statements find_min(const Array *arr, int *min_value, int *min_index)
{
    if (arr->length == 0)
        return invalid_array;

    *min_value = arr->data[0];
    *min_index = 0;

    for (int i = 1; i < arr->length; ++i)
    {
        if (arr->data[i] < *min_value)
        {
            *min_value = arr->data[i];
            *min_index = i;
        }
    }
    return correct;
}

statements find_most_common(const Array *arr, int *result, int *_cnt)
{
    if (arr->length == 0)
    {
        return invalid_array;
    }

    *result = arr->data[0];
    *_cnt = 1;

    for (int i = 0; i < arr->length; ++i)
    {
        int current_value = arr->data[i];
        int cnt = 1;

        for (int j = i + 1; j < arr->length; ++j)
        {
            if (arr->data[j] == current_value)
                cnt++;
        }

        if (cnt > *_cnt || (cnt == *_cnt && current_value > *result))
        {
            *result = current_value;
            *_cnt = cnt;
        }
    }
    return correct;
}

statements find_average(const Array *arr, double *result)
{
    if (arr->length == 0)
    {
        return invalid_array;
    }

    int sum = 0;
    for (int i = 0; i < arr->length; ++i)
    {
        sum += arr->data[i];
    }

    *result = (double)sum / arr->length;

    return correct;
}

statements find_max_distance_average(const Array *array, double *max_deviation)
{
    if (array->length == 0)
    {
        return invalid_array;
    }

    double average;
    statements status_average = find_average(array, &average);
    if (status_average != correct)
        return status_average;

    *max_deviation = 0;
    int mins = INT_MAX, maxs = INT_MIN;

    for (int i = 0; i < array->length; ++i)
    {
        mins = min(mins, array->data[i]);
        maxs = max(maxs, array->data[i]);
    }
    *max_deviation = max(fabs(mins - average), fabs(maxs - average));
    return correct;
}

void clear_all_arrays(Array *arr[], size_t siz)
{
    if (arr == NULL){
        return;
    }
    for (size_t i = 0; i < siz; i++)
    {
        if (arr[i] != NULL)
        {
            free_array(&arr[i]);
            free(arr[i]);
        }
    }
}

statements clean_up(FILE *file, char *line, Array *arrs[], statements state)
{
    fclose(file);
    free(line);
    if (arrs != NULL)
    {
        clear_all_arrays(arrs, SYMBS);
    }
    return state;
}


void print_stats(const Array *arr)
{
    int max_value, max_index, min_value, min_index, most_common_value, most_common_count;
    double average, max_deviation;

    statements status_max = find_max(arr, &max_value, &max_index);
    statements status_min = find_min(arr, &min_value, &min_index);
    statements status_most_common = find_most_common(arr, &most_common_value, &most_common_count);
    statements status_average = find_average(arr, &average);
    statements status_deviation = find_max_distance_average(arr, &max_deviation);

    printf("Array Stats %c :\n", arr->name);

    if (status_max == correct)
        printf("Maximum value: %d at index %d\n", max_value, max_index);
    else
        print_status(status_max);

    if (status_min == correct)
        printf("Minimum value: %d at index %d\n", min_value, min_index);
    else
        print_status(status_min);

    if (status_most_common == correct)
        printf("Most common value: %d (occurs %d times)\n", most_common_value, most_common_count);
    else
        print_status(status_most_common);

    if (status_average == correct)
        printf("Average: %.2f\n", average);
    else
        print_status(status_average);

    if (status_deviation == correct)
        printf("Max deviation from average: %.2f\n", max_deviation);
    else
        print_status(status_deviation);
}

statements parsing(const char *filename_pars)
{
    FILE *file = fopen(filename_pars, "r");
    if (file == NULL)
    {
        return invalid_file;
    }

    Array *arrays[SYMBS] = {NULL};

    char *line = NULL;
    int buf_siz = 0;
    int counter = 0;

    statements state;
    int c;

    while ((c = fgetc(file)) != EOF)
    {
        if (line == NULL || counter >= buf_siz)
        {
            buf_siz += 128;
            char *tmp = realloc(line, buf_siz);
            if (tmp == NULL)
            {
                fclose(file);
                free(line);
                return allocate_error;
            }
            line = tmp;
        }
        c = tolower(c);

        line[counter++] = c;

        if (c != ';')
        {
            continue;
        }

        line[counter] = '\0';
        counter = 0;

        char *command = strtok(line, " (\n\t");
        if (command == NULL)
        {
            return clean_up(file, line, arrays, invalid_command);
        }

        if (strcmp(command, "load") == 0)
        {
            char *current_arr = strtok(NULL, ", \n\t");

            if (current_arr == NULL || strlen(current_arr) != 1 || !isalpha(*current_arr))
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            char *path = strtok(NULL, " ;\n\t");

            if (path == NULL || diff_file(filename_pars, path) != correct)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            if (strtok(NULL, " ,;\n\t()") != NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            Array *arr = NULL;

            int ind = *current_arr - 'a';

            if (arrays[ind] != NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }
            if ((state = create_array_s(&arr, *current_arr, 7)) != correct)
            {
                return clean_up(file, line, arrays, state);
            }

            arrays[ind] = arr;
            if ((state = load_array_file(arr, path)) != correct)
            {
                return clean_up(file, line, arrays, state);
            }
        }
        else if (strcmp(command, "save") == 0)
        {
  
            char *current_arr = strtok(NULL, ", \n\t");
            if (current_arr == NULL || strlen(current_arr) != 1 || !isalpha(*current_arr))
            {
                return clean_up(file, line, arrays, invalid_command);
            }
            char *path = strtok(NULL, " ;\n\t");
            if (path == NULL || diff_file(filename_pars, path) != correct)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            if (strtok(NULL, " ,;\n\t()") != NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            Array *arr = NULL;
            int ind = *current_arr - 'a';
            if (arrays[ind] == NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            statements state = dump_array_tofile(arrays[ind], path);
            if (state != correct)
            {
                return clean_up(file, line, arrays, invalid_command);
            }
        }
        else if (strcmp(command, "rand") == 0)
        {
            char *current_arr = strtok(NULL, ", \n\t");
            if (current_arr == NULL || strlen(current_arr) != 1 || !isalpha(*current_arr))
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int count, left, right;

            char *str_cnt = strtok(NULL, ", \n\t");
            state = _better_attoi(str_cnt, &count);

            if (state != correct || count <= 0)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            char *lb_str = strtok(NULL, ", \n\t");
            state = _better_attoi(lb_str, &left);

            if (state != correct)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            char *rb_str = strtok(NULL, "; \n\t");
            state = _better_attoi(rb_str, &right);

            if (state != correct)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            if (strtok(NULL, " ,;\n\t()") != NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int ind = *current_arr - 'a';
            if (arrays[ind] == NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            state = random_fill_array(arrays[ind], count, left, right);
            if (state != correct)
            {
                return clean_up(file, line, arrays, invalid_command);
            }
        }
        else if (strcmp(command, "concat") == 0)
        {
            char *left_arr = strtok(NULL, ", \n\t");
            if (left_arr == NULL || strlen(left_arr) != 1 || !isalpha(*left_arr))
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            char *right_arr = strtok(NULL, "; \n\t");
            if (right_arr == NULL || strlen(right_arr) != 1 || !isalpha(*right_arr))
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            if (strtok(NULL, " ,;\n\t()") != NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int left_ind = *left_arr - 'a';
            int right_ind = *right_arr - 'a';

            if (arrays[left_ind] == NULL || arrays[right_ind] == NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            state = arr_cat(arrays[left_ind], arrays[right_ind]);
            if (state != correct)
            {
                return clean_up(file, line, arrays, invalid_command);
            }
        }
        else if (strcmp(command, "free") == 0)
        {
            char *cur_arr = strtok(NULL, ", )\n\t");
            if (cur_arr == NULL || strlen(cur_arr) != 1 || !isalpha(*cur_arr))
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            if (strtok(NULL, " ,;\n\t()") != NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int index = *cur_arr - 'a';

            if (arrays[index] == NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }
            free_array(&arrays[index]);
        }
        else if (strcmp(command, "remove") == 0)
        {
            char *cur_arr = strtok(NULL, ", \n\t");
            if (cur_arr == NULL || strlen(cur_arr) != 1 || !isalpha(*cur_arr))
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            char *stt_ptr = strtok(NULL, ", \n\t");
            char *end_ptr = strtok(NULL, ", \n\t;");

            if (stt_ptr == NULL || end_ptr == NULL || strtok(NULL, " ,;\n\t()") != NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int index = *cur_arr - 'a';

            if (arrays[index] == NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int start, end;
            statements a = _better_attoi(stt_ptr, &start);
            statements b = _better_attoi(end_ptr, &end);

            if (a != correct || b != correct)
            {
                fclose(file);
                free(line);
                clear_all_arrays(arrays, SYMBS);
                return (a != correct) ? a : b;
            }
            state = remove_elements(arrays[index], start, end);

            if (state != correct)
            {
                return clean_up(file, line, arrays, state);
            }
        }
        else if (strcmp(command, "copy") == 0)
        {
            char *from = strtok(NULL, ", \n\t");
            if (from == NULL || strlen(from) != 1 || !isalpha(*from))
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            char *st_ptr = strtok(NULL, ", \n\t");
            char *end_ptr = strtok(NULL, ", \n\t");
            char *to = strtok(NULL, " ,;\n\t");

            if (st_ptr == NULL || end_ptr == NULL || to == NULL || strtok(NULL, " ,;\n\t()") != NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int from_ind = *from - 'a';
            int to_ind = *to - 'a';

            if (arrays[from_ind] == NULL || arrays[to_ind] == NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int start, end;
            statements a = _better_attoi(st_ptr, &start);
            statements b = _better_attoi(end_ptr, &end);

            if (a != correct || b != correct)
            {
                return clean_up(file, line, arrays, invalid_command);
            }
            state = copy_elements(arrays[from_ind], arrays[to_ind], start, end);

            if (state != correct)
            {
                return clean_up(file, line, arrays, state);
            }
        }
        else if (strcmp(command, "sort") == 0)
        {
            char *arr_name = strtok(NULL, ", \n\t");
            char *sort_type = strtok(NULL, " ;\n\t");

            if (arr_name == NULL || sort_type == NULL || strtok(NULL, " ,;\n\t()") != NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int index = *arr_name - 'a';

            if (arrays[index] == NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            if (strlen(sort_type) != 1 || sort_type[0] != '+' || sort_type[0] != '-')
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            state = sort_array(arrays[index], sort_type[0]);

            if (state != correct)
            {
                return clean_up(file, line, arrays, state);
            }
        }
        else if (strcmp(command, "shuffle") == 0)
        {
            char *cur_arr = strtok(NULL, ", \n\t");

            if (cur_arr == NULL || strtok(NULL, " ,;\n\t()") != NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int ind = *cur_arr - 'a';

            if (arrays[ind] == NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            if ((state = shuffle_array_s(arrays[ind])) != correct)
            {
                return clean_up(file, line, arrays, state);
            }
        }
        else if (strcmp(command, "stats") == 0)
        {
            char *cur_arr = strtok(NULL, ", \n\t;");

            if (cur_arr == NULL || strtok(NULL, " ,;\n\t()") != NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int ind = *cur_arr - 'a';

            if (arrays[ind] == NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            print_stats(arrays[ind]);
        }
        else if (strcmp(command, "print") == 0)
        {
            char *cur_arr = strtok(NULL, ", \n\t");
            if (cur_arr == NULL || strlen(cur_arr) != 1 || !isalpha(*cur_arr))
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int ind = *cur_arr - 'a';

            if (arrays[ind] == NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            char *first = strtok(NULL, ", \n\t;");
            if (first == NULL)
            {
                return clean_up(file, line, arrays, invalid_command);
            }

            int start = 0, end = 0;
            char *second = strtok(NULL, ", \n\t;");

            if (strcmp(first, "all") == 0) // mas all
            {
                if (print_elements(arrays[ind]) != correct)
                {
                    return clean_up(file, line, arrays, invalid_command);
                }
            }
            else if (second == NULL) // mas pos
            {
                int curr_ind;
                state = _better_attoi(first, &curr_ind);
                if (state != correct)
                {
                    return clean_up(file, line, arrays, state);
                }
                state = print_element(arrays[ind], curr_ind);
                if (state != correct)
                {
                    return clean_up(file, line, arrays, state);
                }
            }
            else // mas fir sec
            {
                statements a = _better_attoi(first, &start);
                statements b = _better_attoi(second, &end);

                if (a != correct || b != correct)
                {
                    return clean_up(file, line, arrays, (a == correct) ? b : a);
                }

                state = print_range(arrays[ind], start, end);
                if (state != correct)
                {
                    return clean_up(file, line, arrays, state);
                }
            }
        }

        free(line);
        line = NULL;
        buf_siz = 0;
    }

    fclose(file);
    free(line);
    clear_all_arrays(arrays, 26);
    return correct;
}