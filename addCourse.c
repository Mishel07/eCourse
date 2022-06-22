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

void addCourse(char *courseId, char *courseName, char *facultyName)
{

    FILE *log;

    log = fopen("course.txt", "a");
    if (log == NULL)
    {
        fputs("Error at opening File!", stderr);
        exit(1);
    }

    struct Course c;
    strcpy(c.courseId, courseId);
    strcpy(c.courseName, courseName);
    strcpy(c.facultyName, facultyName);

    fwrite(&c, sizeof(c), 1, log);
    fclose(log);

    printf("\nCourse Registration Successful!\n");
}
