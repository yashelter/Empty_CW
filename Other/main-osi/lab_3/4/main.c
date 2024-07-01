#include "validate.c"

void print_usage()
{
    printf("Usage: ./main.out -p <input_file> -m <new_attributes> \n");
    printf("-b for word format (w, r, x)\n");
}

int main(int argc, char *argv[])
{
    if (!valid(argc, argv))
    {
        printf("Invalid usage!\n");
        print_usage();
        return 1;
    }

    mode_t new_permissions = 0;
    char *endptr;
    if (argc == 6 && argv[5][1] == 'b')
    {
        endptr = argv[4];
        while (*endptr != '\0')
        {
            switch (*(endptr))
            {
            case 'r':
                new_permissions |= S_IRUSR;
                break;
            case 'w':
                new_permissions |= S_IWUSR;
                break;
            case 'x':
                new_permissions |= S_IXUSR;
                break;
            default:
                printf("Invalid permission flag: %c\n", *argv[4]);
                print_usage(argv[0]);
                return 1;
            }
            endptr++;
        }
    }
    else
    {
        new_permissions = strtol(argv[4], &endptr, 8);
    }

    if (*endptr != '\0')
    {
        printf("Invalid usage!\n");
        print_usage();
        return 1;
    }

    if (chmod(argv[2], new_permissions) == 0)
    {
        printf("Атрибуты файла %s успешно изменены.\n", argv[2]);
    }
    else
    {
        perror("Ошибка при изменении атрибутов файла");
        return 1;
    }

    return 0;
}
