#include "poisson.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// calculate n! (n * (n-1) * (n-2) * ... * 1)
int factorial(int n) {
    if (n == 0) {
        return 1;
    }
    int result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

// calculate the Poisson distribution for a given k and 𝛌
long double poisson(int k, double lambda) {
    return (pow(lambda, k) / factorial(k)) * expl(-lambda);
}