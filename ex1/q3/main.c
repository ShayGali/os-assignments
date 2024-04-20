

#include <stdio.h>
#include <string.h>

#include "poisson.h"

int main(void) {
    printf("\n~~~~~~~~~~~~ Poisson distribution ~~~~~~~~~~~~\n\n");

    printf("       𝛌       |       k       |     P_X(k) \n");
    printf("---------------|---------------|---------------\n");
    printf("       2       |       1       |%.12Lf\n", poisson(1, 2));
    printf("       2       |       10      |%.12Lf\n", poisson(10, 2));
    printf("       2       |       2       |%.12Lf\n", poisson(2, 2));
    printf("       3       |       3       |%.12Lf\n", poisson(3, 3));
    printf("      100      |       3       |%.12Lf\n", poisson(3, 100));
    return 0;
}