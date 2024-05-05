#include <stdio.h>
#include <stdlib.h>

#include "max_sub_array.h"

// define the range of the random elements
#define MAX_ELEMENT 74
#define MIN_ELEMENT -25

void generate_random_array(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % (MAX_ELEMENT - MIN_ELEMENT + 1) + MIN_ELEMENT;
    }
}

int main(int argc, char const *argv[]) {
    // get the seed and n from the arguments
    int seed;
    int n;
    if (argc != 3) {
        printf("Usage: %s <seed> <n>\n", argv[0]);
        return 1;
    }
    if (sscanf(argv[1], "%d", &seed) != 1) {
        printf("Error: seed should be an integer\n");
        return 1;
    }

    if (sscanf(argv[2], "%d", &n) != 1) {
        printf("Error: n should be an integer\n");
        return 1;
    }

    srand(seed);  // set the seed for the random number generator
    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
        printf("Error while allocating memory for the array\n");
        return 1;
    }

    generate_random_array(arr, n);

    printf("The maximum subarray sum is %d\n", max_sub_array(arr, n));

    free(arr);
    return 0;
}
