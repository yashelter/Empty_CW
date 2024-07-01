#include "program.c"

#define HSIZE 128

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Arguments are input and output paths\n");
        return correct;
    }
    statements stm = validate(argv[1], argv[2]);
    
    if (stm != correct)
    {
        return 1;
    }

    Hash_table *hashtable = create_hash_table(HSIZE);

    if (hashtable == NULL)
    {
        printf("Error occured uwu\n");
    }

    stm = parsing_file(&hashtable, argv[1], argv[2]);

    delete_table(hashtable);

    if (stm != correct)
    {
        printf("Occured error with code (%d)\n", stm);
    }
    else
    {
        printf("All went good\n");
    }

    return 0;
}