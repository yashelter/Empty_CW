#include "work4.h"

void print_hint()
{
    printf("==Program made by Gubarev Mikhail==\n\n");
    printf("=============A few info: ==========\n");
    printf("> Supported flags : [d, i, s, a] and n option available for them\n ");
    printf("> Flag symbol is - or / \n");
    printf("\n\n>>> Program Output<<<\n\n");
}


int main(int argc, char* argv[])
{
    print_hint();
    bool is_new_file;
    char flag;
    enum input_statements stm = check_input(argc, argv, &flag, &is_new_file);

    if (stm != input_correct)
    {
        printf("<< Error : input invalid\n");
        return 0;
    }

    char *out_path;

    if (is_new_file && try_to_open_w(argv[3]) != input_correct) 
    {
        printf("<< Error : invalid path\n");
        return 0;
    }

    if (is_new_file) {out_path = create_output_file(argv[2]); }
    else { out_path = argv[3]; }

    stm = input_invalid;
    
    if (flag == 'a') 
    {
        stm = flag_a(argv[2], out_path);
    }
    if (flag == 's') 
    {
        stm = flag_s(argv[2], out_path);
    }
    if (flag == 'i') 
    {
        stm = flag_i(argv[2], out_path);
    }
    if (flag == 'd') 
    {
        stm = flag_d(argv[2], out_path);
    }

    free(out_path);
    if (stm == input_correct)
    {
        printf("<< All done correctly\n");
    }
    else
    {
        printf("<< Error : check input\n");
    }


}