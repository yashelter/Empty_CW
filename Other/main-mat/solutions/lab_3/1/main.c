#include "app.c"


int main(){
    int a = 110;
    char *s;
    number_to_cc(a, 2, &s);
    printf("%s\n", s);
}