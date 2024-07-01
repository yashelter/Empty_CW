#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_S 4096

void list_files(const char *dirname)
{
    DIR *dir;
    struct dirent *entry;

    // Открытие каталога
    dir = opendir(dirname);

    if (dir == NULL)
    {
        printf("Catalog not exist or can't be opened\n");
        return;
    }

    // Чтение файлов в каталоге
    while ((entry = readdir(dir)) != NULL)
    {
        char path[MAX_S];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        struct stat file_stat;
        if (stat(path, &file_stat) == -1)
        {
            printf("Error with getting info about file\n");
            closedir(dir);
            return;
        }

        printf("%-20s ", entry->d_name);
        if (S_ISDIR(file_stat.st_mode))
        {
            printf("Directory\n");
        }
        else if (S_ISREG(file_stat.st_mode))
        {
            printf("FILE\n");
        }
        else if (S_ISLNK(file_stat.st_mode))
        {
            printf("Link\n");
        }
        else
        {
            printf("Unknown type\n");
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage is : %s <catalog(s)>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; ++i)
    {
        printf("Catalog insde %s:\n", argv[i]);
        list_files(argv[i]);
        printf("\n\n");
    }

    return 0;
}
