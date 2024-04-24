#include "max_sub_array.h"

/**
 * @brief Find the maximum subarray sum in n^3 time complexity
 *
 * A straightforward way to solve the problem is to go through all possible subarrays,
 * calculate the sum of values in each subarray and maintain the maximum sum.
 */
int max_sub_array(int arr[], int n) {
    int best = 0;

    for (int a = 0; a < n; a++) {      // start point
        for (int b = a; b < n; b++) {  // end point
            int sum = 0;
            for (int k = a; k <= b; k++) {  // sum the values in the subarray
                sum += arr[k];
            }

            // update the best sum if we found a better one
            if (sum > best)
                best = sum;
        }
    }
    return best;
}