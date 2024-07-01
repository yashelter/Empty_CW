#include "work4.h"

int main(int argc, char *argv[])
{
    print_hint();
    statements stm = check_input(argc, argv);
    if (stm != correct)
    {
        print_err(stm);
        return 0;
    }

    FILE *file;
    stm = try_open_file(&file, argv[1], "rb");
    if (stm != correct)
    {
        print_err(stm);
        return 0;
    }
    bool flag = false;
    if (strcmp(argv[2], "mask") == 0)
    {
        char hex_num[4];
        int l = strlen(argv[3]);

        for (int i = 0; i < 4; i++)
        {
            if (i < l && is_hex(argv[3][i]))
            {
                char c = argv[3][i];
                if ('0' <= c && c <= '9')
                {
                    c -= '0';
                }
                else if ('A' <= c && c <= 'F')
                {
                    c -= 'A' + 10;
                }
                hex_num[3 - i] = c;
            }
            else
            {
                flag = true;
                hex_num[3 - i] = 0;
            }
        }
        mask_hex(file, hex_num);
    }
    else if (strcmp(argv[2], "xor8") == 0)
    {
        xor8(file);
    }
    else if (strcmp(argv[2], "xor32") == 0)
    {
        xor32(file);
    }
    fclose(file);
    if (flag)
    {
        printf("Some numbers in mask was invalid, so they was ingored\n");
    }
}