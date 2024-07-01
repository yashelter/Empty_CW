#include "app.c"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <input_file>", argv[0]);
        return 1;
    }

    char *out = NULL;
    statements result = run(argv[1], &out);
    if (result == correct)
    {
        printf("Check file %s\n", out);
    }
    printf("Exit code : %d\n", result); 
    exit_state(result);
    if (out != NULL)
    {
        free(out);
    }

    return result;
}