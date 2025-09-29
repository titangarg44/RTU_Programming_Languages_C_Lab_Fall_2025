/*
 * Lab 3, Task 2
 * Student Name:- Sandeep Garg, Student ID:- 241ADB010
 *
 * Practice using pointers as function parameters.
 * Implement:
 *   - swap (exchange values of two ints)
 *   - modify_value (multiply given int by 2)
 *
 * Rules:
 *   - Use pointers to modify variables in the caller.
 *   - Demonstrate changes in main.
 *
 * Example:
 *   int a = 5, b = 10;
 *   swap(&a, &b);   // now a = 10, b = 5
 *
 *   modify_value(&a); // now a = 20
 */

#include <stdio.h>

// Function prototypes
void swap(int *x, int *y);
void modify_values(int *x, int *y);

int main(void) {
    int a = 3, b = 7;
    printf("Before swap: a=%d, b=%d\n", a, b);
    swap(&a, &b);
    printf("After swap: a=%d, b=%d\n", a, b);

    modify_values(&a, &b);
    printf("After modify_value: a=%d\n b=%d", a,b);

    return 0;
}

// Implement functions below
void swap(int *x, int *y) {
    int t;
     t   = *x;
    *x   = *y;
    *y   =  t;
    // TODO: swap values using a temporary variable
}

void modify_values(int *x, int *y) {
    *x *= 2;
    *y *= 2;
    // TODO: multiply value by 2
}