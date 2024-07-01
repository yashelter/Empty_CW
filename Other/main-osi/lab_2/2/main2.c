#include <unistd.h>
#include <stdio.h>

int main()
{
    fork(); // программа делится на 2
    printf("Hi\n"); // 2 вывода

    fork(); // каждая делённая на 2 ещё раз делится на 2
    printf("Hi\n"); // 4 вывода
    // итого 6 * Hi
}

