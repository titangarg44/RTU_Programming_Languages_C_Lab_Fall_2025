// week5_task2_struct_save_load.c
// Task 2: Save and load structured records from a file
// Week 5 – Files & Modular Programming
// TODO: Complete function implementations and file handling logic.
// week5_task2_struct_save_load.c
// Task 2: Save and load structured records from a file
// Week 5 – Files & Modular Programming
// TODO: Complete function implementations and file handling logic.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 50

typedef struct {
    char name[MAX_NAME_LEN];
    int age;
    float gpa;
} Student;

// Function prototypes
void save_student(Student s, const char *filename);
Student load_student(const char *filename);

int main(void) {
    Student s1;
    strcpy(s1.name, "Alice");
    s1.age = 21;
    s1.gpa = 3.75f;

    const char *filename = "student.txt";
    
    save_student(s1,filename);
    
    printf("Loaded Student: \n");
    printf("Name: %s\n", s1.name);
    printf("Age: %d\n", s1.age);
    printf("GPA: %.2f\n", s1.gpa);
    
    return 0;
}

void save_student(Student s, const char *student) {
    FILE *fp = fopen(student, "w");
    if (fp == NULL){
        printf("Error opening the file for writing\n");
        exit(1);
    }
    fprintf(fp, "%s %d %.2f\n", s.name, s.age, s.gpa);
    fclose(fp);
}

Student load_student(const char *student) {
    Student s;
    FILE*fp = fopen(student,"r");
    if (fp == NULL){
        printf("Error opening the file for reading.\n");
        exit(1);
    }
    fscanf(fp, "%s %d %f", s.name, &s.age, &s.gpa);
    fclose(fp);
    return s;
}
