#include <stdio.h>
#include <stdlib.h> // For EXIT_SUCCESS/FAILURE, which might be good practice
#include <string.h> // For string functions like strcpy

// Maximum number of students the array can hold
#define MAX_STUDENTS 100
// Maximum length of a student's name
#define NAME_LEN 50
// Name of the data file
#define DATA_FILE "students.txt"

// Student structure definition
typedef struct {
    char name[NAME_LEN];
    int id;
    float gpa;
} Student;

// function prototypes
// Loads students from the file, returns number of records loaded
int load_students(Student arr[]);
// Saves all students to the file
void save_students(Student arr[], int count);
// Adds a new student record
void add_student(Student arr[], int *count);
// Prints all student records to the console
void list_students(Student arr[], int count);

// --- MAIN FUNCTION ---
int main(void) {
    // Array to hold student records
    Student students[MAX_STUDENTS];
    // Current number of students in the array, initialized to 0
    int count = 0;
    // User's menu choice
    int choice;

    // TODO: load existing data from file using load_students()
    count = load_students(students);
    printf("Loaded %d student record(s) from file.\n\n", count);

    do {
        printf("--- Student Management System ---\n");
        printf("1. List students\n");
        printf("2. Add student\n");
        printf("3. Save and Exit\n");
        printf("4. Exit without Saving\n");
        printf("---------------------------------\n");
        printf("Select an option: ");

        // Read user choice
        if (scanf("%d", &choice) != 1) {
            // Handle non-integer input
            choice = 0; // Set to an invalid option
            while (getchar() != '\n'); // Clear the input buffer
        } else {
            while (getchar() != '\n'); // Clear the newline from the buffer
        }

        switch (choice) {
            case 1:
                // TODO: Call list_students()
                list_students(students, count);
                break;
            case 2:
                // TODO: Call add_student()
                add_student(students, &count);
                break;
            case 3:
                // TODO: Call save_students() and exit loop
                save_students(students, count);
                printf("All student records saved to %s.\n", DATA_FILE);
                break;
            case 4:
                printf("Exiting without saving.\n");
                break;
            default:
                printf("\n** Invalid option. Try again. **\n\n");
                break;
        }

    } while (choice != 3 && choice != 4);

    return 0;
}

// --- FUNCTION DEFINITIONS ---

// Open DATA_FILE, read records until EOF, return number of records loaded
int load_students(Student arr[]) {
    FILE *fp;
    int records_loaded = 0;

    // Open the file for reading ("r")
    fp = fopen(DATA_FILE, "r");

    // Check if the file opened successfully
    if (fp == NULL) {
        // If the file doesn't exist yet, it's not an error.
        // Just return 0 to indicate no records were loaded.
        return 0;
    }

    // Read student records from the file until EOF (End Of File) is reached
    // Assuming records are stored as "name id gpa" separated by newlines
    while (records_loaded < MAX_STUDENTS &&
           fscanf(fp, "%s %d %f",
                  arr[records_loaded].name,
                  &arr[records_loaded].id,
                  &arr[records_loaded].gpa) == 3) {
        records_loaded++;
    }

    // Close the file
    fclose(fp);

    return records_loaded;
}



// Write all students to DATA_FILE
void save_students(Student arr[], int count) {
    FILE *fp;
    int i;

    // Open the file for writing ("w"). This creates the file if it doesn't exist
    // or truncates (clears) the file if it does exist.
    fp = fopen(DATA_FILE, "w");

    // Check if the file opened successfully
    if (fp == NULL) {
        perror("Error opening file for saving");
        return;
    }

    // Write each student record to the file
    for (i = 0; i < count; i++) {
        // Write as: name id gpa\n
        fprintf(fp, "%s %d %.2f\n",
                arr[i].name,
                arr[i].id,
                arr[i].gpa);
    }

    // Close the file
    fclose(fp);
}



// Read input from user and append to array
void add_student(Student arr[], int *count) {
    // Check for array overflow
    if (*count >= MAX_STUDENTS) {
        printf("\n** ERROR: Student array is full. Cannot add more students. **\n");
        return;
    }

    printf("\n--- Add New Student ---\n");

    // Get Name
    printf("Enter Name (one word): ");
    // Use %s to read a single word.
    // Use an explicit width limit to prevent buffer overflow.
    scanf("%49s", arr[*count].name);

    // Get ID
    printf("Enter ID: ");
    while (scanf("%d", &arr[*count].id) != 1) {
        printf("Invalid input. Please enter an integer for ID: ");
        while (getchar() != '\n');
    }
    // Clear newline character from buffer
    while (getchar() != '\n');

    // Get GPA
    printf("Enter GPA: ");
    while (scanf("%f", &arr[*count].gpa) != 1 || arr[*count].gpa < 0.0 || arr[*count].gpa > 4.0) {
        printf("Invalid input. Please enter a GPA between 0.0 and 4.0: ");
        while (getchar() != '\n');
    }
    // Clear newline character from buffer
    while (getchar() != '\n');

    // Increment the count of students
    (*count)++;

    printf("Student record added successfully!\n\n");
}


// Print all student records
void list_students(Student arr[], int count) {
    int i;

    printf("\n--- Student List (%d Records) ---\n", count);

    if (count == 0) {
        printf("No students currently in the system.\n");
        printf("---------------------------------\n\n");
        return;
    }

    // Print Header
    printf("%-5s | %-20s | %s\n", "ID", "Name", "GPA");
    printf("----------------------------------------\n");

    // Print each student record
    for (i = 0; i < count; i++) {
        printf("%-5d | %-20s | %.2f\n",
               arr[i].id,
               arr[i].name,
               arr[i].gpa);
    }

    printf("----------------------------------------\n\n");
}