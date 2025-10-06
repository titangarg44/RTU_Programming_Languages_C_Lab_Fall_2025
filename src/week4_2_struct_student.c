/*
 * week4_2_struct_student.c
 * Author: Sandeep Garg
 * Student ID: 241ADB010
 * Description:
 *   Demonstrates defining and using a struct in C.
 *   Students should define a 'Student' struct with fields like name, id, and grade.
 *   Then create a few instances and print them.
 */

#include <stdio.h>
#include <string.h>

// Define struct Student with fields: name (char[]), id (int), grade (float)
struct Student {
    char name[50];
    int id;
    float grade;
};

int main(void) {
    // Declare one Student variable
    struct Student s1;

    // Assign values (via user input)
    printf("Enter student name: ");
    scanf("%49s", s1.name);  // %49s prevents buffer overflow

    printf("Enter student ID: ");
    scanf("%d", &s1.id);

    printf("Enter student grade: ");
    scanf("%f", &s1.grade);

    // Print struct contents
    printf("\n--- Student Details ---\n");
    printf("Name: %s\n", s1.name);
    printf("ID: %d\n", s1.id);
    printf("Grade: %.2f\n", s1.grade);

    return 0;
}
