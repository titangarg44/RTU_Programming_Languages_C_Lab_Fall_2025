#include <stdio.h>

/*
    Task:
    Write a function `long long factorial(int n)` that computes n!
    using a loop (not recursion).

    In main():
      - Ask user for an integer n
      - If n is negative, print an error and exit
      - Otherwise, call factorial and print the result
*/

long long factorial(int n) {
    // TODO: compute factorial iteratively
    int fact = 1;
    for(int i=1; i<=n; i++){
        fact *= i;
    }
    return fact; // placeholder
}

int main(void) {
    int n;

    printf("Enter a non-negative integer n: ");
    scanf("%d", &n);
    if (n<0){
        printf("Incorrect! You can't have a negative number factorial.");
    }
    else{
        printf("Factorial of n:%d",factorial(n));
    }

    // TODO: validate input, call function, print result

    return 0;
}