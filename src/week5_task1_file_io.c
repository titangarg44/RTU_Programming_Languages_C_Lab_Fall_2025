// week5_task1_file_io.c
// Task 1: Read and write data from text files
// Week 5 – Files & Modular Programming
// TODO: Fill in the missing parts marked below.

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  FILE* fp;
  char filename[100] = "car.txt";
  char line[256];

  // Open a file in read mode
  fp = fopen("car.txt", "a+");

  if (fp == NULL) {
    printf("Error opening the file!");
    return 1;
  }
  while (fgets(line, sizeof(line), fp)) {
    printf("%s", line);
  }
  //writing
  fprintf(fp, "Nice\n");
  // renaming
  char new_name[100];
  printf("Enter the file new name: ");
  scanf("%s", new_name);
  rename("car.txt", new_name);

  fclose(fp);
  return 0;
}