#include "max_sub_array.h"
/**
 * @brief Find the maximum subarray sum in n time complexity
 *
 * now in each iteration, the sum will be the maximum of the current element (arr[k])
 * and the maximum subarray sum ending at the previous element (sum + arr[k]).
 *
 * and from this, we will take the maximum of the current subarray (max(arr[k], sum + arr[k])) and the best sum so far.
 */
int max_sub_array(int arr[], int n) {
    int best = 0, sum = 0;

    for (int k = 0; k < n; k++) {
        if (arr[k] > (sum + arr[k]))
            sum = arr[k];
        else
            sum += arr[k];

        if (sum > best)
            best = sum;
    }

    return best;
}