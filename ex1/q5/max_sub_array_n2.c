#include "max_sub_array.h"

/**
 * @brief Find the maximum subarray sum in n^2 time complexity
 * the difference between this function and the n^3 version is that we are updating the sum
 * in the inner loop instead of calculating the sum of the subarray in each iteration.
 */
int max_sub_array(int arr[], int n) {
    int best = 0;
    for (int a = 0; a < n; a++) {  // start point
        int sum = 0;
        for (int b = a; b < n; b++) {    // end point
            sum += arr[b];               // sum of arr[a:b]
            if (sum > best) best = sum;  // update the best sum if we found a better one
        }
    }
    return best;
}