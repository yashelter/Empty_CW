#include "polish_notation.c"

statements run(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        return invalid_file;
    }
    char out[200] = "";
    strcat(out, path);
    strcat(out, "_out.txt");
    FILE *out_file = fopen(out, "w");

    if (out == NULL)
    {
        fclose(file);
        return invalid_file;
    }
    int counter = 0;
    while (!feof(file))
    {
        printf("\n");
        counter++;
        int cnt = 0;
        char *preprocessed;
        char *line = scan_line(file, &cnt);
        if (line == NULL)
        {
            break;
        }
        fprintf(stdout, "entered_notation : %s,\n", line);
        statements stm = solve(line, &preprocessed);

        if (stm != correct)
        {
            fprintf(out_file, "number : %d\n",counter);
            fprintf(out_file, "getted_problem_code : %d\n", stm);
            free(line);
            continue;
        }
        fprintf(stdout, "builded_notation : %s,\n", preprocessed);
        int result;

        stm = calculate(preprocessed, &result);

        if (stm != correct)
        {
            fprintf(out_file, "number : %d\n", counter);
            fprintf(out_file, "getted_error_calculating_code : %d\n", stm);
            free(preprocessed);
            free(line);
            continue;
        }
        fprintf(stdout, "correct : true,\n");
        fprintf(stdout, "calculated_value : %d\n", result);
        free(preprocessed);
        free(line);
    }
    fclose(out_file);
    fclose(file);
}

statements app(char *st[], int n)
{
    for (int i = 1; i < n; i++)
    {
        printf("File : <%s> :\n", st[i]);
        statements state = run(st[i]);
        printf("eof\n\n");
    }
    
}