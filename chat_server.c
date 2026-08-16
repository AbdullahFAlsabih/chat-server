#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#include "chat_server.h"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

// static char chat_history[1 * 1024]= "testing";

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    // memset(chat_history, 0, (1 * 1024));

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <portno>\n", argv[0]);
        exit(1);
    }

    int sockfd, portno, newsockfd;
    portno = atoi(argv[1]);

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Error opening socket");

    // Bind
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Binding failed");

    // Listen
    listen(sockfd, 5);
    printf("Server started on port %d\n", portno);

    // Client list
    Clients clients[MAX_CLIENTS];
    int client_count = 0;

    // Initialize client sockets to 0
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].client_sockets = 0;
        clients[i].username = NULL;
    }

    char buffer[BUFFER_SIZE];
    fd_set read_fds;

    while (1)
    {
        // Clear and set the fd set
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);

        int max_fd = sockfd;

        // Add client sockets to the set
        for (int i = 0; i < client_count; i++)
        {
            int fd = clients[i].client_sockets;
            FD_SET(fd, &read_fds);
            if (fd > max_fd)
                max_fd = fd;
        }

        // Wait for activity
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0)
            error("select error");

        // Check if there's a new connection
        if (FD_ISSET(sockfd, &read_fds))
        {
            clilen = sizeof(cli_addr);
            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
            if (newsockfd < 0)
                error("Error on accept");

            // Add to client list
            clients[client_count++].client_sockets = newsockfd;
            printf("New client connected. Total: %d\n", client_count);

            // usernamev messge
            char username_msg[] = "Enter your username (upto 18 characters max)):";
            write(newsockfd, username_msg, strlen(username_msg));

            // reading username
            bzero(buffer, BUFFER_SIZE);
            int read_bytes = read(newsockfd, buffer, BUFFER_SIZE - 1);

            clients[client_count - 1].username = buffer;

            // Welcome message
            char *welcome_msg;
            sprintf(welcome_msg, "Welcome to the chat %s\n", clients[client_count - 1].username);
            write(newsockfd, welcome_msg, strlen(welcome_msg));

            // Chat history
            // char history_print[2 * 1024];
            // sprintf(history_print, "Old Messages:-\n %s\nNew Messages:-\n", chat_history);
            // write(newsockfd, history_print, strlen(history_print));
        }

        // Check all clients for incoming messages
        for (int i = 0; i < client_count; i++)
        {
            int client_fd = clients[i].client_sockets;

            if (FD_ISSET(client_fd, &read_fds))
            {
                bzero(buffer, BUFFER_SIZE);
                int n = read(client_fd, buffer, BUFFER_SIZE - 1);

                if (n <= 0)
                {
                    // Client disconnected
                    printf("Client %d disconnected.\n", client_fd);
                    close(client_fd);

                    // Remove from list
                    for (int j = i; j < client_count - 1; j++)
                    {
                        clients[j].client_sockets = clients[j + 1].client_sockets;
                    }
                    client_count--;
                    i--; // Recheck the current index
                }
                else
                {
                    // Remove newline
                    buffer[strcspn(buffer, "\n")] = 0;

                    if (strcmp(buffer, "bye") == 0)
                    {
                        write(client_fd, "Goodbye!\n", 9);
                        close(client_fd);
                        // Remove from list
                        for (int j = i; j < client_count - 1; j++)
                        {
                            clients[j].client_sockets = clients[j + 1].client_sockets;
                        }
                        client_count--;
                        i--;
                    }
                    else
                    {
                        // Broadcast to all other clients
                        char msg[BUFFER_SIZE + 20];
                        sprintf(msg, "%s: %s\n", clients[i].username, buffer);
                        for (int j = 0; j < client_count; j++)
                        {
                            if (clients[j].client_sockets != client_fd)
                            {
                                write(clients[j].client_sockets, msg, strlen(msg));
                            }
                        }
                        // strncat(chat_history, msg, sizeof(&chat_history) - strlen(chat_history) - 1);
                    }
                }
            }
        }
    }

    close(sockfd);
    return 0;
}