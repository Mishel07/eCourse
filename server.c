#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "header.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080  // main server port
#define PORT1 8081 // group 1 port
#define PORT2 8082 // group 2 port

void main()
{

    int subProcess1, subProcess2; // To create process for group 1 and group 2

    int pipe1[2], pipe2[2];

    int sockfd, clientSock, recieveVal, sendMsg, groupPort, groupPort1, groupPort2;
    int opt = 1;

    // Keeps address information
    struct sockaddr_in sockAddr;
    char recClientMsg[1024] = {0}; // buffer
    int sockAddrLen = sizeof(sockAddr);

    // creating pipe 1 for group 1 and for main server communication
    if (pipe(pipe1) < 0) // pipe > 0 then success
    {
        fprintf(stderr, "Pipe 1 Failed");
        exit(EXIT_FAILURE);
    }

    // creating pipe 2 for group 2 and for main server communication
    if (pipe(pipe2) < 0)
    {
        fprintf(stderr, "Pipe 2 Failed");
        exit(EXIT_FAILURE);
    }

    subProcess1 = fork();

    if (subProcess1 > 0)
    {
        // creating subprocess 1 using fork() for group 1
        subProcess2 = fork();

        // parent process
        if (subProcess2 > 0)
        {

            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) // Success > 0
            {
                perror("\nSocket creation failed at server");
                exit(EXIT_FAILURE);
            }

            printf("\nMain Server Socket successfully created..");

            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
            {
                perror("\nsetsockopt error at server");
                exit(EXIT_FAILURE);
            }

            // assign IP, PORT
            sockAddr.sin_family = AF_INET;
            sockAddr.sin_addr.s_addr = INADDR_ANY;
            sockAddr.sin_port = htons(PORT); // Main server port 8080

            // binding newly created socket to given IP and verification
            if (bind(sockfd, (struct sockaddr *)&sockAddr, sockAddrLen) < 0)
            {
                perror("\nBind failed at server...");
                exit(EXIT_FAILURE);
            }

            printf("\nMain Server Socket successfully binded...");

            // now main server is ready to listen and verification
            if (listen(sockfd, 5) < 0)
            {
                perror("\nMain Server Listening failed...");
                exit(EXIT_FAILURE);
            }
            printf("\nMain Server listening...");

            while (1)
            {

                close(pipe1[1]);

                // read string from pipe1 and store under group port 1 and close reading end.
                read(pipe1[0], &groupPort1, sizeof(groupPort1)); // Group 1 port 8081
                close(pipe1[0]);

                close(pipe2[1]);

                // read string from pipe2 and store under group port 2 and close reading end.
                read(pipe2[0], &groupPort2, sizeof(groupPort2)); // Group 2 port 8082
                close(pipe2[0]);

                printf("\nPort number of Group 1 is %d", groupPort1);
                printf("\nPort number of Group 2 is %d", groupPort2);

                if ((clientSock = accept(sockfd, (struct sockaddr *)&sockAddr, &sockAddrLen)) < 0)
                {
                    perror("cant accept at server");
                    exit(EXIT_FAILURE);
                }

                printf("\nClient Accepted in Main Server...");

                // fourth time read GROUP to join message from the client
                recieveVal = read(clientSock, recClientMsg, 1024);
                if (recieveVal < 0)
                {
                    perror("Could not recieve message");
                    exit(EXIT_FAILURE);
                }

                if (strcmp(recClientMsg, "1") == 0)
                {
                    groupPort = groupPort1;
                    printf("\nClient: %s\n", recClientMsg);
                }
                else if (strcmp(recClientMsg, "2") == 0)
                {
                    groupPort = groupPort2;
                    printf("\nClient: %s\n", recClientMsg);
                }
                else
                {
                    printf("Invalid group\n");
                }
                memset(recClientMsg, 0, 1024);

                // sending port of the group server to client for connecting to that group server
                if (write(clientSock, &groupPort, sizeof(groupPort)) < 0)
                {
                    perror("Coud not send port to client...");
                }
                // display the message sent to the client
                printf("Port sent to client : %d\n", groupPort);
            }
        }
        else if (subProcess2 == 0)
        {
            // group 2
            // variables declaration
            int child2Sock, child2ClientSock;
            struct sockaddr_in child2SocketAddress;
            int child2SocketAddressLength = sizeof(child2SocketAddress);
            int group2Port = 8082;
            int recieveVal, sendMsg;
            char recClientMsg[1024] = {0};

            // creating socket for group 2 file descriptor
            if ((child2Sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                perror("\nSocket creation failed for group 2...");
                exit(EXIT_FAILURE);
            }

            printf("\nGroup 2 Server Socket successfully created...");

            // forcefully attaching socket for group 2  to the port 8080
            if (setsockopt(child2Sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
            {
                perror("\nSetsockopt error at group 2 server");
                exit(EXIT_FAILURE);
            }

            // assign IP, PORT
            child2SocketAddress.sin_family = AF_INET;
            child2SocketAddress.sin_addr.s_addr = INADDR_ANY;
            child2SocketAddress.sin_port = htons(PORT2);

            // binding newly created socket to given IP and verification
            if (bind(child2Sock, (struct sockaddr *)&child2SocketAddress, child2SocketAddressLength) < 0)
            {
                perror("\nBind failed at group 2 server");
                exit(EXIT_FAILURE);
            }

            printf("\nGroup 2 Server Socket successfully binded...");

            // now server is ready to listen and verification
            if (listen(child2Sock, 5) < 0)
            {
                perror("\nGroup 2 Server Listening failed");
                exit(EXIT_FAILURE);
            }
            printf("\nGroup 2 Server listening...");

            while (1)
            {
                // sending port number to main server and then close the read end of pipe 2
                close(pipe2[0]);

                // group 2 server port is sent to main server using the write method and close the write end of pipe 2
                write(pipe2[1], &group2Port, sizeof(group2Port));
                close(pipe2[1]);

                // accepting the client that have arrived and connect to the group 2 server
                // as the code is in loop so it can be accept another client also
                if ((child2ClientSock = accept(child2Sock, (struct sockaddr *)&child2SocketAddress, &child2SocketAddressLength)) < 0)
                {
                    perror("\nCant accept at group 2 server");
                    exit(EXIT_FAILURE);
                }

                printf("\nClient Accepted in Grpup 2 Server...");

                while (1)
                {
                    // reads the message sent by client
                    recieveVal = read(child2ClientSock, recClientMsg, 1024);

                    if (recieveVal < 0)
                    {
                        perror("\nCould not recieve message in group  1");
                        exit(EXIT_FAILURE);
                    }

                    if (*recClientMsg == '1')
                    {
                        recieveVal = read(child2ClientSock, recClientMsg, 1024);
                        if (recieveVal < 0)
                        {
                            perror("\nCould not recieve message in group  1");
                            exit(EXIT_FAILURE);
                        }

                        char username[50];
                        strcpy(username, recClientMsg);
                        printf("\nusername %s ", username);

                        recieveVal = read(child2ClientSock, recClientMsg, 1024);
                        if (recieveVal < 0)
                        {
                            perror("\nCould not recieve message in group  1");
                            exit(EXIT_FAILURE);
                        }

                        char password[50];
                        strcpy(password, recClientMsg);
                        printf("\npassword %s ", password);

                        registration(username, password);
                        char *msg = "Registration successful";
                        sendMsg = send(child2ClientSock, msg, strlen(msg), 0);
                    }
                    if (*recClientMsg == '2')
                    {
                    label2:
                        memset(recClientMsg, 0, 1024);
                        recieveVal = read(child2ClientSock, recClientMsg, 1024);
                        if (recieveVal < 0)
                        {
                            perror("\nCould not recieve message in group  1");
                            exit(EXIT_FAILURE);
                        }

                        char username[50];
                        strcpy(username, recClientMsg);
                        printf("\nusername %s ", username);

                        memset(recClientMsg, 0, 1024);
                        recieveVal = read(child2ClientSock, recClientMsg, 1024);
                        if (recieveVal < 0)
                        {
                            perror("\nCould not recieve message in group  1");
                            exit(EXIT_FAILURE);
                        }

                        char password[50];
                        strcpy(password, recClientMsg);
                        printf("\npassword %s ", password);
                        printf("\n");

                        if (login(username, password))
                        {
                            printf("\nsuccess");
                            printf("\n");

                            char *msg = "1";

                            sendMsg = send(child2ClientSock, msg, strlen(msg), 0);
                            while (1)
                            {
                                memset(recClientMsg, 0, 1024);
                                recieveVal = read(child2ClientSock, recClientMsg, 1024);
                                if (recieveVal < 0)
                                {
                                    perror("\nCould not recieve message in group  1");
                                    exit(EXIT_FAILURE);
                                }
                                printf("%s", recClientMsg);
                                printf("\n");
                                if (*recClientMsg == '1')
                                {
                                    memset(recClientMsg, 0, 1024);
                                    recieveVal = read(child2ClientSock, recClientMsg, 1024);
                                    if (recieveVal < 0)
                                    {
                                        perror("\nCould not recieve message in group  1");
                                        exit(EXIT_FAILURE);
                                    }

                                    char courseId[50];
                                    strcpy(courseId, recClientMsg);
                                    printf("\ncourseId %s ", courseId);
                                    printf("\n");

                                    memset(recClientMsg, 0, 1024);
                                    recieveVal = read(child2ClientSock, recClientMsg, 1024);
                                    if (recieveVal < 0)
                                    {
                                        perror("\nCould not recieve message in group  1");
                                        exit(EXIT_FAILURE);
                                    }

                                    char courseName[50];
                                    strcpy(courseName, recClientMsg);
                                    printf("\ncourseName %s ", courseName);
                                    printf("\n");

                                    memset(recClientMsg, 0, 1024);
                                    recieveVal = read(child2ClientSock, recClientMsg, 1024);
                                    if (recieveVal < 0)
                                    {
                                        perror("\nCould not recieve message in group  1");
                                        exit(EXIT_FAILURE);
                                    }

                                    char facultyName[50];
                                    strcpy(facultyName, recClientMsg);
                                    printf("\nfacultyName %s ", facultyName);
                                    printf("\n");
                                    addCourse(courseId, courseName, facultyName);
                                }
                                else if (*recClientMsg == '2')
                                {
                                    msg = FileRead();
                                    sendMsg = send(child2ClientSock, msg, strlen(msg), 0);
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }
                        else
                        {
                            printf("\nfail");
                            printf("\n");

                            char *msg = "0";
                            sendMsg = send(child2ClientSock, msg, strlen(msg), 0);
                            goto label2;
                        }
                    }
                    else if (*recClientMsg == '3')
                    {

                        // if it is DISCONNECT client will get disconnected from group 2
                        printf("\nClient is disconnected from the Group 2");
                        break;
                    }

                    memset(recClientMsg, 0, 1024);
                }
            }
        }
    }
    else if (subProcess1 == 0)
    {

        // group 1
        // variables declaration
        int child1Sock, child1ClientSock;
        struct sockaddr_in child1SockAddr;
        int child1SockAddrLen = sizeof(child1SockAddr);
        int group1Port = 8081;
        int recieveVal, sendMsg;
        char recClientMsg[1024] = {0};
        char *group1Buffer = "";     // buffer is used to store chat
        group1Buffer = malloc(5000); // initialize buffer dynamically

        // creating socket for group 2 file descriptor
        if ((child1Sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("\nSocket creation failed for group 1...");
            exit(EXIT_FAILURE);
        }

        printf("\nGroup 1 Server Socket successfully created...");

        // forcefully attaching socket for group 2  to the port 8080
        if (setsockopt(child1Sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            perror("\nSetsockopt error at group 1 server");
            exit(EXIT_FAILURE);
        }

        // assign IP, PORT
        child1SockAddr.sin_family = AF_INET;
        child1SockAddr.sin_addr.s_addr = INADDR_ANY;
        child1SockAddr.sin_port = htons(PORT1);

        // binding newly created socket to given IP and verification
        if (bind(child1Sock, (struct sockaddr *)&child1SockAddr, sockAddrLen) < 0)
        {
            perror("\nBind failed at group 1 server");
            exit(EXIT_FAILURE);
        }

        printf("\nGroup 1 Server Socket successfully binded...");

        // now server is ready to listen and verification
        if (listen(child1Sock, 5) < 0)
        {
            perror("\nGroup 1 Server Listening failed");
            exit(EXIT_FAILURE);
        }
        printf("\nGroup 1 Server listening...");

        while (1)
        {

            // sending port number to main server and then close the read end of pipe 1
            close(pipe1[0]);

            // group 1 server port is sent to main server using the write method and close the write end of pipe 1
            write(pipe1[1], &group1Port, sizeof(group1Port));
            close(pipe1[1]);

            // accepting the client that have arrived and connect to the group 1 server
            // as the code is in loop so it can be accept another client also

            if ((child1ClientSock = accept(child1Sock, (struct sockaddr *)&child1SockAddr, &sockAddrLen)) < 0)
            {
                perror("\nCant accept at group 1 server");
                exit(EXIT_FAILURE);
            }
            printf("Client Accepted in Grpup 1 Server...");

            printf("\nClient Accepted in Grpup 1 Server...");

            while (1)
            {
                // reads the message sent by client
                recieveVal = read(child1ClientSock, recClientMsg, 1024);

                if (recieveVal < 0)
                {
                    perror("\nCould not recieve message in group  1");
                    exit(EXIT_FAILURE);
                }

                if (*recClientMsg == '1')
                {
                    recieveVal = read(child1ClientSock, recClientMsg, 1024);
                    if (recieveVal < 0)
                    {
                        perror("\nCould not recieve message in group  1");
                        exit(EXIT_FAILURE);
                    }

                    char username[50];
                    strcpy(username, recClientMsg);
                    printf("\nusername %s ", username);

                    recieveVal = read(child1ClientSock, recClientMsg, 1024);
                    if (recieveVal < 0)
                    {
                        perror("\nCould not recieve message in group  1");
                        exit(EXIT_FAILURE);
                    }

                    char password[50];
                    strcpy(password, recClientMsg);
                    printf("\npassword %s ", password);

                    registration(username, password);
                    char *msg = "Registration successful";
                    sendMsg = send(child1ClientSock, msg, strlen(msg), 0);
                }
                else if (*recClientMsg == '2')
                {
                label1:
                    memset(recClientMsg, 0, 1024);
                    recieveVal = read(child1ClientSock, recClientMsg, 1024);
                    if (recieveVal < 0)
                    {
                        perror("\nCould not recieve message in group  1");
                        exit(EXIT_FAILURE);
                    }

                    char username[50];
                    strcpy(username, recClientMsg);
                    printf("\nusername %s ", username);

                    memset(recClientMsg, 0, 1024);
                    recieveVal = read(child1ClientSock, recClientMsg, 1024);
                    if (recieveVal < 0)
                    {
                        perror("\nCould not recieve message in group  1");
                        exit(EXIT_FAILURE);
                    }

                    char password[50];
                    strcpy(password, recClientMsg);
                    printf("\npassword %s ", password);

                    if (login(username, password))
                    {
                        printf("\nsuccess");
                        printf("\n");

                        char *msg = "1";

                        sendMsg = send(child1ClientSock, msg, strlen(msg), 0);
                        memset(recClientMsg, 0, 1024);
                        while (1)
                        {
                            recieveVal = read(child1ClientSock, recClientMsg, 1024);
                            if (recieveVal < 0)
                            {
                                perror("\nCould not recieve message in group  1");
                                exit(EXIT_FAILURE);
                            }
                            printf("%s", recClientMsg);
                            printf("\n");
                            if (*recClientMsg == '1')
                            {
                                msg = FileRead();
                                sendMsg = send(child1ClientSock, msg, strlen(msg), 0);
                            }
                            // enroll course....
                            else if (*recClientMsg == '2')
                            {
                            label3:
                                memset(recClientMsg, 0, 1024);
                                recieveVal = read(child1ClientSock, recClientMsg, 1024);
                                if (recieveVal < 0)
                                {
                                    perror("\nCould not recieve message in group  1");
                                    exit(EXIT_FAILURE);
                                }
                                printf("\nmsg=%s", recClientMsg);

                                char courseId[50];
                                strcpy(courseId, recClientMsg);
                                printf("\ncourseId = %s", courseId);
                                printf("\n");

                                char *msg = "accept";
                                sendMsg = send(child1ClientSock, msg, strlen(msg), 0);
                                recieveVal = read(child1ClientSock, recClientMsg, 1024);
                                if (recieveVal < 0)
                                {
                                    perror("\nCould not recieve message in group  1");
                                    exit(EXIT_FAILURE);
                                }
                                printf("\nmsg=%s", recClientMsg);
                                char studentName[50];
                                strcpy(studentName, recClientMsg);
                                printf("\nstudentName = %s ", studentName);
                                printf("\n");
                                if (enroll(courseId, studentName))
                                {
                                    printf("\nsuccess");
                                    printf("\n");
                                    char *msg = "1";
                                    sendMsg = send(child1ClientSock, msg, strlen(msg), 0);
                                }
                                else
                                {
                                    printf("\nfail");
                                    printf("\n");
                                    char *msg = "0";
                                    sendMsg = send(child1ClientSock, msg, strlen(msg), 0);
                                    goto label3;
                                }
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        printf("\nfail");
                        printf("\n");

                        char *msg = "0";
                        sendMsg = send(child1ClientSock, msg, strlen(msg), 0);
                        goto label1;
                    }
                }

                // checking if client wrote DISCONNECT
                if (!strcmp(recClientMsg, "DISCONNECT"))
                {
                    // if it is DISCONNECT client will get disconnected from group 1
                    printf("\nClient is disconnected from the Group 1");
                    break;
                }
                // reseting recServerMsg to empty
                memset(recClientMsg, 0, 1024);
            }
        }
    }
}
