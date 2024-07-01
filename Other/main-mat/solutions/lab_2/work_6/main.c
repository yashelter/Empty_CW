#include "work6.c"

int main ()
{
    int res = 0;
    int res2 = 0;
    int res3 = 0;
    //ss_to_base_10("zzz", 36, &res, false);
    
    oversscanf("010101011 1 2", "%Zr %d %d", &res, &res2, &res3);
    printf("\n%d, %d, %d\n", res, res2, res3);
}