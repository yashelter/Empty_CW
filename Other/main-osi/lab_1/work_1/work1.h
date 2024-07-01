#include <stdio.h>

typedef enum
{
    invalid,
    correct,
    invalid_input,
    error_file
} statements;


statements validate_input(int argc, char *argv[])
{
    if (argc != 2)
    {
        return invalid_input;
    }
    return correct;
}

statements try_to_open(FILE **file, char *path, char *param)
{
    *file = fopen(path, param);
    if (file == NULL)
    {
        printf("Error : invalid file path or file opened\n");
        return error_file;
    }
    return correct;
}


statements task1(FILE *output)
{
    char bytes_mass[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    fwrite(bytes_mass, sizeof(char),  11, output);
    return correct;
}


statements task2(FILE *output)
{
    char bytic;
    printf("\nReading file by bytes : \n\n");
    while (fread(&bytic, sizeof(char), 1, output) == 1) 
    {
        printf("\nReaded byte is : ");
        printf("%d\n\n", bytic);
        printf("High-order word is _IO_MAGIC; rest is flags : %d\n", output->_flags);
        printf("Current read pointer : %p\n", output->_IO_read_ptr);
        printf("End of get area : %p\n", output->_IO_read_end);
        printf("Start of putback+get area : %p\n", output->_IO_read_base);
        printf("Start of put area : %p\n", output->_IO_write_base);
        printf("Current put pointer : %p\n", output->_IO_write_ptr);
        printf("End of put area : %p\n", output->_IO_write_end);
        printf("Start of reserve area : %p\n", output->_IO_buf_base);
        printf("End of reserve area : %p\n", output->_IO_buf_end);

        printf("Pointer to start of non-current get area : %p\n", output->_IO_save_base);
        printf("Pointer to first valid character of backup area : %p\n", output->_IO_backup_base);
        printf("Pointer to end of non-current get area : %p\n", output->_IO_save_end);

        printf("_fileno : %d\n", output->_fileno);
        printf("_flags2 : %d\n", output->_flags2);

        printf("_cur_column : %d\n", output->_cur_column);
        printf("_vtable_offset : %hhd\n", output->_vtable_offset);
        printf("_shortbuf[1] : %s\n", output->_shortbuf);
    }
    return correct;

}


statements task3(FILE **input)
{
    if (fseek(*input, 3, SEEK_SET) != 0)
    {
        printf("Can't do fseek\n");
        fclose(*input);
        return error_file;
    }
    return correct;
}

statements task4(FILE **input, char buf[])
{
    if (fread(buf, sizeof(char), 4, *input) != 4)
    {
        printf("Can't read 4 bytes\n");
        fclose(*input);
        return error_file;
    }
    return correct;
}

void print_hint()
{
    printf("\t==Program made by Gubarev Mikhail==\n\n");
    printf("\t============A few info: ===========\n");
    printf("\n\n");
    printf("\t  >>>>> Program Output <<<<<\n\n");
}