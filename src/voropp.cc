#include "voropp.h"

double array_sum(double* arr, size_t length) {
    double sum = 0.0;
    for (size_t i = 0; i < length; ++i) {
        sum += arr[i];
    }
    return sum;
}
