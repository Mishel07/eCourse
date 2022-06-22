#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

struct login
{
    char username[30];
    char password[20];
};

int login(char *username, char *password)
{
    FILE *log;

    struct login l;

    log = fopen("login.txt", "r");
    if (!log)
    {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    int flag = 0;
    while (fread(&l, sizeof(l), 1, log))
    {
        if (strcmp(username, l.username) == 0 && strcmp(password, l.password) == 0)
        {
            flag = 1;
            break;
        }
    }

    if (flag == 0)
    {
        // printf("\nIncorrect Login Details\nPlease enter the correct credentials\n");
        return 0;
    }
    else if (flag == 1)
    {
        // printf("Successfully login");
        // printf("\n");
        return 1;
    }
    fclose(log);
}
