#include "hash_table.c"

char *normalize_path(const char *path)
{
    char *normalized = realpath(path, NULL);
    return normalized;
}

statements validate(const char *in, const char *out)
{
    char *input = normalize_path(in);
    char *outpt = normalize_path(out);

    if (input == NULL || outpt == NULL)
    {
        free(input);
        free(outpt);
        return -1;
    }
    bool is_ok = (strcmp(input, outpt) == 0);

    free(input);
    free(outpt);

    if (is_ok)
    {
        printf("Error : input and output paths are equal\n");
        return invalid_file;
    }
    return correct;

}