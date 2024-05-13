#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// calculate n! (n * (n-1) * (n-2) * ... * 1)
long long factorial(int n) {
    if (n == 0) {
        return 1;
    }
    long long result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

long double poisson(int k, double lambda) {
    return (pow(lambda, k) / factorial(k)) * expl(-lambda);
}

int main(int argc, char const *argv[]) {
    // check for correct number of arguments
    if (argc != 3) {
        printf("Usage: ./Poisson <𝛌> <k>\n");
        return 1;
    }
    // get lambda and k from command line arguments
    double lambda;
    int k;
    if (sscanf(argv[1], "%lf", &lambda) != 1) {
        printf("𝛌 need to be a number\n");
        return 1;
    }

    // check if k is an integer
    float temp;
    if (sscanf(argv[2], "%d", &k) != 1 || sscanf(argv[2], "%f", &temp) != 1 || temp != k) {
        printf("k need to be an integer\n");
        return 1;
    }

    long double val = poisson(k, lambda);
    printf("P_X(%d) = %.10Lf\n", k, val);
    return 0;
}