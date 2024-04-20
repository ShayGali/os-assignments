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
    printf("%lld\n", factorial(k));
    printf("%F\n", pow(lambda, k));
    printf("%LF\n", expl(-lambda));
    return (pow(lambda, k) / factorial(k)) * expl(-lambda);
}
int main(int argc, char const *argv[]) {
    // check for correct number of arguments
    if (argc != 3) {
        printf("Usage: ./Poisson <lambda> <k>\n");
        return 1;
    }
    // get lambda and k from command line arguments
    double lambda = atof(argv[1]);
    int k = atoi(argv[2]);
    printf("lambda = %f, k = %d\n", lambda, k);
    long double val = poisson(k, lambda);
    printf("P_X(%d) = %.10Lf\n", k, val);
    return 0;
}