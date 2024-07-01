#include <stdio.h>


int main()
{
    int originalNumber = 123456789;
    int pos = 0;
    const char *originalString = "Hello, World!";
    char charArray[sizeof(int) * 100];

    // Записываем число и строку в массив char
    write_int_array(charArray, &pos, originalNumber);
    write_int_array(charArray, &pos, 12312);
    

    // Записываем строку
    snprintf(charArray + pos, sizeof(charArray) - pos, "%s", originalString);
    // snprintf(array + *position, 15, "%s", string);


    pos = 0;

    // Считываем число и строку из массива char
    int readNumber1 = readNumberFromCharArray(charArray, &pos);
    int readNumber2 = readNumberFromCharArray(charArray, &pos);
    const char *readString = readStringFromCharArray(charArray);

    // Печатаем считанные числа и строку
    printf("Read number 1: %d\n", readNumber1);
    printf("Read number 2: %d\n", readNumber2);
    printf("Read string: %s\n", readString);

    return 0;
}