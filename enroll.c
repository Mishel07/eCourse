#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

struct enroll
{
    char course_id[50];
    char courseName[50];
    char studentName[50];
    char facultyName[50];
};

struct Course
{
    char courseId[50];
    char courseName[100];
    char facultyName[50];
};

int enroll(char *courseId, char *studentName)
{
    struct enroll e;
    struct Course c;
    int flag = 0;
    FILE *log;
    FILE *log1;
    printf("\nhello");
    printf("\n");
    printf("\nhie");
    printf("\n");
    strcpy(e.course_id, courseId);
    strcpy(e.studentName, studentName);
    log1 = fopen("course.txt", "r");
    if (log1 == NULL)
    {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }
    while (fread(&c, sizeof(c), 1, log1))
    {
        if (strcmp(courseId, c.courseId) == 0)
        {
            strcpy(e.courseName, c.courseName);
            strcpy(e.facultyName, c.facultyName);
            flag = 1;
            break;
        }
    }
    fclose(log1);

    log = fopen("enroll.txt", "a");
    if (log == NULL)
    {
        fputs("Error at opening File!", stderr);
        exit(1);
    }
    if (flag == 0)
    {
        printf("\nCourse not present...");
        printf("\n");
        return 0;
    }
    else if (flag == 1)
    {
        fwrite(&e, sizeof(struct enroll), 1, log);
        if (fwrite != NULL)
            printf("Contents to file written successfully !\n");
        else
            printf("Error writing file !\n");
        printf("\nData : \nCourse Id : %s \nCourse name : %s \nStudent Name : %s \nFaculty Name : %s", e.course_id, e.courseName, e.studentName, e.facultyName);
        printf("\n");
        printf("\nEnrolled in course successfully...");
        printf("\n");
        return 1;
    }
    fclose(log);
}