#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

struct login
{
    char username[30];
    char password[20];
};

void registration(char *username, char *password)
{
    char firstname[15];
    FILE *log;

    log = fopen("login.txt", "a");
    if (log == NULL)
    {
        fputs("Error at opening File!", stderr);
        exit(1);
    }

    struct login l;
    strcpy(l.username, username);
    strcpy(l.password, password);

    fwrite(&l, sizeof(l), 1, log);
    fclose(log);

    printf("\nRegistration Successful!\n");
}
