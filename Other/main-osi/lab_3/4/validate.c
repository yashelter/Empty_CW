#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

bool is_flag(char *s)
{
    return (strlen(s) == 2 && s[0] == '-');
}

bool valid(int argc, char *argv[])
{
    if ((argc != 5 && argc != 6) || argv[1][0] != '-' || argv[3][0] != '-')
    {
        return false;
    }
    char *flag = argv[1];
    char *file_path = argv[2];

    if (!is_flag(argv[1]) || argv[1][1] != 'p')
    {
        return false;
    }
    if (!is_flag(argv[3]) || argv[3][1] != 'm')
    {
        return false;
    }
    if ((argc == 6) && (!is_flag(argv[5]) || argv[5][1] != 'b'))
    {
        return false;
    }
    return true;
}