#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

void main()
{
    // variables declaration
    int sock, recieveVal, sendMsg;
    struct sockaddr_in sockAddr;
    char recServerMsg[1024] = {0};
    char *msg=malloc(sizeof(char)*1024),*username=malloc(sizeof(char)*1024);
    int sockAddrLen = sizeof(sockAddr);
    int portForJoin;

    // socket create and verification
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("\nSocket creation failed at server");
        exit(EXIT_FAILURE);
    }

    // assign IP, PORT
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &sockAddr.sin_addr) <= 0)
    {
        perror("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    // connect the client socket to main server socket
    if (connect(sock, (struct sockaddr *)&sockAddr, sockAddrLen) < 0)
    {
        printf("\nConnection Failed at the client end...");
        exit(EXIT_FAILURE);
    }

        printf("\nWelcome to eCourse");
        printf("\n1. Student");
        printf("\n2. Faculty");
        printf("\n3. Exit");
        printf("\nEnter your choice: ");
        gets(msg);
        if (*msg == '3')
        {
            printf("\nClient exits..");
            printf("\n");
            exit(0);
        }

        // send group no message to server through send method
        sendMsg = send(sock, msg, strlen(msg), 0);

        // check whether message sent or not
        if (sendMsg < 0)
        {
            perror("\nCould not send message to server...");
            exit(EXIT_FAILURE);
        }

        // informing client that you are connected to this group
        printf("\nYou are connected to the server");

        // checking whether the port is recieved from main server or not
        if (read(sock, &portForJoin, sizeof(portForJoin)) < 0)
        {
            perror("\nError: Reading group port");
            exit(EXIT_FAILURE);
        }

        // displays the response
        printf("\nPort no Received from server: %d", portForJoin);

        // closing the main server connection
        close(sock);
        printf("\nClosing the main server connection of client...");

        // creating new connection for sub server
        sock = socket(AF_INET, SOCK_STREAM, 0);

        // reinitialization the address
        sockAddr.sin_family = AF_INET;

        // initialising the port recieved and connecting to the group server
        sockAddr.sin_port = htons(portForJoin);

        // convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &sockAddr.sin_addr) < 0)
        {
            perror("\nInvalid address/ Address not supported...");
            exit(EXIT_FAILURE);
        }

        // connecting to the group server
        if (connect(sock, (struct sockaddr *)&sockAddr, sockAddrLen) < 0)
        {
            printf("\nConnection Failed at client...");
            exit(EXIT_FAILURE);
        }

        printf("\nClient Connected to Group Server\n");

        // reseting recServerMsg to empty
        memset(recServerMsg, 0, 1024);

        if (*msg == '1')
        {
            printf("\nstudent");
            while (1)
            {
                printf("\n1. Register \n2. Login \n3. Exit \nEnter your choice: ");
                gets(msg);
                sendMsg = send(sock, msg, strlen(msg), 0);

                if (*msg == '1')
                {
                    memset(msg, 0, 1024);
                    printf("Enter your username: ");
                    gets(msg);

                    sendMsg = send(sock, msg, strlen(msg), 0);
                    memset(msg, 0, 1024);

                    printf("Enter your password: ");
                    gets(msg);

                    sendMsg = send(sock, msg, strlen(msg), 0);
                    memset(msg, 0, 1024);
                    recieveVal = read(sock, recServerMsg, 1024);
                    if (recieveVal < 0)
                    {
                        perror("\nCould not recieve message from server");
                        exit(EXIT_FAILURE);
                    }
                    printf("\n%s", recServerMsg);
                }

                else if (*msg == '2')
                {
                label1:
                    printf("\nEnter your username: ");
                    gets(msg);
                    strcpy(username,msg);

                    sendMsg = send(sock, msg, strlen(msg), 0);
                    memset(msg, 0, 1024);

                    printf("Enter your password: ");
                    gets(msg);

                    sendMsg = send(sock, msg, strlen(msg), 0);
                    memset(msg, 0, 1024);

                    recieveVal = read(sock, recServerMsg, 1024);
                    if (recieveVal < 0)
                    {
                        perror("\nCould not recieve message from server");
                        exit(EXIT_FAILURE);
                    }

                    if (*recServerMsg == '0')
                    {
                        memset(recServerMsg, 0, 1024);
                        printf("\nInvalid");
                        goto label1;
                    }
                    else
                    {
                        printf("\nLogin successful");
                        while (1)
                        {
                            printf("\n1. View course \n2. Enroll course \n3. Exit \nEnter your choice");
                            gets(msg);
                            if (*msg == '1')
                            {
                                sendMsg = send(sock, msg, strlen(msg), 0);
                                memset(msg, 0, 1024);
                                recieveVal = read(sock, recServerMsg, 1024);
                                if (recieveVal < 0)
                                {
                                    perror("\nCould not recieve message from server");
                                    exit(EXIT_FAILURE);
                                }
                                printf("\n%s", recServerMsg);
                                //fileRead();
                            }
                            //enroll course..
                            else if(*msg=='2'){
                                sendMsg = send(sock, msg, strlen(msg), 0);
                                label3:
                                memset(msg, 0, 1024);
                                printf("\nEnter course ID to enroll: ");
                                gets(msg);
                                sendMsg = send(sock, msg, strlen(msg), 0);
                                memset(msg, 0, 1024);
                                recieveVal = read(sock, recServerMsg, 1024);
                                strcpy(msg,username);
                                
                                sendMsg = send(sock, msg, strlen(msg), 0);
                                memset(msg, 0, 1024);
                                
                                recieveVal = read(sock, recServerMsg, 1024);
                                if (recieveVal < 0)
                                {
                                    perror("\nCould not recieve message from server");
                                    exit(EXIT_FAILURE);
                                }

                                if (*recServerMsg == '0')
                                {
                                    memset(recServerMsg, 0, 1024);
                                    printf("\ncourse Id not present");
                                    goto label3;
                                }
                                else{
                                    printf("\nCourse enrolled successfully...");
                                }
                            }
                            else if (*msg == '3'){
                                sendMsg = send(sock, msg, strlen(msg), 0);
                                memset(msg, 0, 1024);
                                break;
                            }
                        }
                    }
                }
                else if (*msg == '3')
                {
                    // close(sock);
                    break;
                }
                else
                    printf("\nInvalid choice");
            }
        }
        else if (*msg == '2')
        {
            printf("\nfaculty");
            while (1)
            {
                printf("\n1. Register \n2. Login \n3. Exit \nEnter your choice");
                gets(msg);
                sendMsg = send(sock, msg, strlen(msg), 0);

                if (*msg == '1')
                {
                    memset(msg, 0, 1024);
                    printf("Enter your username: ");
                    gets(msg);

                    sendMsg = send(sock, msg, strlen(msg), 0);
                    memset(msg, 0, 1024);

                    printf("Enter your password: ");
                    gets(msg);

                    sendMsg = send(sock, msg, strlen(msg), 0);
                    memset(msg, 0, 1024);
                    recieveVal = read(sock, recServerMsg, 1024);
                    if (recieveVal < 0)
                    {
                        perror("\nCould not recieve message from server");
                        exit(EXIT_FAILURE);
                    }
                    printf("\n%s", recServerMsg);
                }
                else if (*msg == '2')
                {
                label2:
                    printf("\nEnter your username: ");
                    gets(msg);

                    sendMsg = send(sock, msg, strlen(msg), 0);
                    memset(msg, 0, 1024);

                    printf("Enter your password: ");
                    gets(msg);

                    sendMsg = send(sock, msg, strlen(msg), 0);
                    memset(msg, 0, 1024);

                    recieveVal = read(sock, recServerMsg, 1024);
                    if (recieveVal < 0)
                    {
                        perror("\nCould not recieve message from server");
                        exit(EXIT_FAILURE);
                    }

                    if (*recServerMsg == '0')
                    {
                        printf("\nInvalid");
                        goto label2;
                    }
                    else
                    {
                        printf("\nLogin successful");
                        while (1)
                        {
                            printf("\n1. Add course \n2. View course \n3. Exit \nEnter your choice");
                            gets(msg);

                            if (*msg == '1')
                            {
                                sendMsg = send(sock, msg, strlen(msg), 0);
                                memset(msg, 0, 1024);
                                printf("\nEnter your course ID : ");
                                gets(msg);
                                sendMsg = send(sock, msg, strlen(msg), 0);
                                memset(msg, 0, 1024);

                                printf("\nEnter your course name : ");
                                gets(msg);

                                sendMsg = send(sock, msg, strlen(msg), 0);
                                memset(msg, 0, 1024);

                                printf("\nEnter faculty name : ");
                                gets(msg);

                                sendMsg = send(sock, msg, strlen(msg), 0);
                                memset(msg, 0, 1024);

                                printf("\nCourse registration successful");
                            }
                            else if(*msg=='2'){
                                sendMsg = send(sock, msg, strlen(msg), 0);
                                memset(msg, 0, 1024);
                                recieveVal = read(sock, recServerMsg, 1024);
                                if (recieveVal < 0)
                                {
                                    perror("\nCould not recieve message from server");
                                    exit(EXIT_FAILURE);
                                }
                                printf("\n%s", recServerMsg);
                            }
                            else if (*msg == '3'){
                                sendMsg = send(sock, msg, strlen(msg), 0);
                                memset(msg, 0, 1024);
                                break;
                            }
                        }
                    }
                }
                else if (*msg == '3')
                {
                    exit(0);
                }
                else
                    printf("\nInvalid choice");
            }
        }
        else
        {
            printf("\nInvalid choice");
            printf("\n");
        }
}
