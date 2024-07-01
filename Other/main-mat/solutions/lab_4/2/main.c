#include "app.c"


int main()
{
    srand(time(NULL));

     statements status = parsing("comands.txt");

    if (status != correct)
    {
        print_status(status);
    }

    return status;
}