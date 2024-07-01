#include "Base.c"


statements create_array_s(Array** arr, char name, int initial_capacity) 
{
    *arr = (Array*)malloc(sizeof(Array));
    if (*arr == NULL) return allocate_error;
    
    (*arr)->data = (int*)malloc(initial_capacity * sizeof(int));
    if ((*arr)->data == NULL) 
    {
        free(*arr);
        return allocate_error;
    }
    (*arr)->name = toupper(name);
    (*arr)->length = 0;
    (*arr)->capacity = initial_capacity;

    return correct;
}

statements free_array(Array** arr) 
{
    if (*arr == NULL) return invalid_array;
    free((*arr)->data);
    (*arr)->data = NULL;
    (*arr)->length = 0;
    (*arr)->capacity = 0;
   // free(*arr);
    
    return correct;
}

statements _better_attoi(const char* str, int* result) 
{
    char* endptr;
    long int converted = strtol(str, &endptr, 10);

    if (endptr == str || (*endptr != '\0' && *endptr != '\n')) 
    {
        return invalid_format;
    }

    if (converted > INT_MAX || converted < INT_MIN) 
    {
        return int_overflow;
    }

    *result = (int)converted;
    return correct;
}

statements load_array_file(Array* array, const char* path) 
{
    FILE* file = fopen(path, "r");
    if (file == NULL) 
    {
        return invalid_file;
    }

    char* buf = NULL;
    size_t buf_siz = 0;

    while (getline(&buf, &buf_siz, file) != -1) 
    {
        char* token = strtok(buf, " \t\n");

        while (token != NULL) 
        {
            int converted_value;
            statements state = _better_attoi(token, &converted_value);

            if (state != correct) 
            {
                fclose(file);
                free(buf);
                return state;
            }

            if (array->length == array->capacity) 
            {
                array->capacity *= 2;
                int* tmp = (int*)realloc(array->data, array->capacity * sizeof(int));

                if (tmp == NULL) 
                {
                    fclose(file);
                    free(buf);
                    return allocate_error;
                }
                array->data = tmp;
            }

            array->data[array->length++] = converted_value;
            token = strtok(NULL, " \t\n");
        }
    }

    fclose(file);
    free(buf);
    return correct;
}


statements print_element(const Array* arr, int position) 
{
    if (position < 0 || position >= arr->length) { return invalid_index; }
    printf("Element at position %d in Array %c: %d\n", position, arr->name, arr->data[position]);

    return correct;
}

statements print_range(const Array* arr, int start, int end) 
{
    if (start < 0 || start >= arr->length || end < start || end >= arr->length) return invalid_index;

    printf("Elements from range [%d, %d] of Array %c: ", start, end, arr->name);
    for (int i = start; i <= end; i++) { printf("%d ", arr->data[i]); }
    printf("\n");

    return correct;
}

statements print_elements(const Array* arr) 
{
    if (arr->length == 0) { return invalid_array;}

    printf("All elements of Array %c: ", arr->name);
    for (int i = 0; i < arr->length; i++) { printf("%d ", arr->data[i]); }
    printf("\n");
    return correct;
}

statements dump_array_tofile(const Array* arr, const char* path) 
{
    FILE* file = fopen(path, "w");
    if (file == NULL) {return invalid_file;}

    for (int i = 0; i < arr->length; i++) { fprintf(file, "%d ", arr->data[i]); }

    fclose(file);
    return correct;
}

// fill with rand vals in range [mins, maxs]
statements random_fill_array(Array* arr, int n, int mins, int maxs) 
{
    if (mins > maxs) {return invalid_erange;}

    if (n > arr->capacity) 
    {
        int* tmp = (int*)realloc(arr->data, n * sizeof(int));
        if (tmp == NULL) {return allocate_error;}
        arr->capacity = n;
        arr->data = tmp;
    }

    srand(time(NULL));

    for (int i = 0; i < n; i++) 
    {
        arr->data[i] = rand() % (maxs - mins + 1) + mins; //от lb до rb
    }

    arr->length = n;

    return correct;
}


// concat b to a
statements arr_cat(Array *a, const Array *b)
{
    if (a->length + b->length > a->capacity)
    {
        int *tmp = (int *)realloc(a->data, (a->length + b->length) * sizeof(int));
        if (tmp == NULL)
        {
            return allocate_error;
        }
        a->capacity = a->length + b->length;
        a->data = tmp;
    }

    for (int i = 0; i < b->length; i++)
    {
        a->data[a->length + i] = b->data[i];
    }

    a->length += b->length;

    return correct;
}

// remove n elements from start_index
statements remove_elements(Array *arr, int ind, int n)
{
    if (ind < 0 || ind >= arr->length || n < 0 || ind + n > arr->length)
    {
        return invalid_erange;
    }

    for (int i = ind; i + n < arr->length; i++)
    {
        arr->data[i] = arr->data[i + n];
    }

    // Обновление длины массива
    arr->length -= n;

    return correct;
}

statements copy_elements(const Array *a, Array *b, int left, int right)
{
    if (left < 0 || left >= a->length || right < left || right >= a->length)
    {
        return invalid_erange;
    }

    if (right - left + 1 > b->length)
    {
        int *tmp = (int *)realloc(b->data, (right - left + 1) * sizeof(int));
        if (tmp == NULL)
        {
            return allocate_error;
        }
        b->capacity = right - left + 1;
        b->length = right - left + 1;
        b->data = tmp;
    }

    for (int i = left; i < right + 1; i++)
    {
        b->data[i] = a->data[i];
    }

    return correct;
}