#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

struct Course
{
    char courseId[50];
    char courseName[100];
    char facultyName[50];
};

char *FileRead()
{
    FILE *log;
    char *courses = malloc(sizeof(char) * 1024);
    struct Course c;

    log = fopen("course.txt", "r");
    if (log == NULL)
    {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    int flag = 0;
    printf("\nCourses Available:");
    strcpy(courses, "CourseId CourseName StudentName FacultyName");
    while (fread(&c, sizeof(c), 1, log))
    {
        strcat(courses, "\n");
        strcat(courses, c.courseId);
        strcat(courses, " ");
        strcat(courses, c.courseName);
        strcat(courses, " ");
        strcat(courses, c.facultyName);
        strcat(courses, " ");
        printf("\n%s %s %s", c.courseId, c.courseName, c.facultyName);
        printf("\n");
    }
    fclose(log);
    return (char *)courses;
}