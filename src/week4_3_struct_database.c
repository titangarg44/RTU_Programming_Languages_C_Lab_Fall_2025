/*
 * week4_3_struct_database.c
 * Author: Sandeep Garg
 * Student ID: 241ADB010
 * Description:
 *   Simple in-memory "database" using an array of structs.
 *   Students will use malloc to allocate space for multiple Student records,
 *   then input, display, and possibly search the data.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define struct Student with fields name, id, grade
struct Student {
    char name[50];
    int id;
    float grade;
};

int main(void) {
    int n;
    struct Student *students = NULL;
    float totalGrade = 0.0;

    printf("Enter number of students: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid number.\n");
        return 1;
    }

    // Allocate memory for n Student structs using malloc
    students = malloc(n * sizeof(struct Student));
    if (students == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Read student data in a loop
    for (int i = 0; i < n; i++) {
        printf("\n--- Student %d ---\n", i + 1);

        printf("Enter name: ");
        scanf("%49s", students[i].name);  // %49s prevents overflow

        printf("Enter ID: ");
        scanf("%d", &students[i].id);

        printf("Enter grade: ");
        scanf("%f", &students[i].grade);

        totalGrade += students[i].grade;
    }

    // Display all student records in formatted output
    printf("\n=== Student Records ===\n");
    for (int i = 0; i < n; i++) {
        printf("Name: %-10s | ID: %5d | Grade: %.2f\n",
               students[i].name, students[i].id, students[i].grade);
    }

    // Optional: Compute average grade
    printf("\nAverage grade: %.2f\n", totalGrade / n);

    // Free allocated memory
    free(students);

    return 0;
}
