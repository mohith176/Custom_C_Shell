#include "iMan.h"
void send_request(int socket_fd, char *argument)
{
    char request[4096];
    snprintf(request, sizeof(request),
             "GET /?topic=%s&section=all HTTP/1.1\r\n"
             "Host: man.he.net\r\n"
             "Connection: close\r\n\r\n",
             argument);
    if (send(socket_fd, request, strlen(request), 0) == -1)
    {
        perror("send\n");
        exit(1);
    }
}

void filter(int socket_fd)
{
    char buffer[4096];
    int header_ended = 0;
    ssize_t received;

    while ((received = recv(socket_fd, buffer, sizeof(buffer)-1, 0)) > 0)
    {
        buffer[received] = '\0';
        char *body = buffer;
        if (!header_ended)
        {
            body = strstr(buffer, "<");
            if (body)
            {
                header_ended = 1;
            }
            else
            {
                continue;
            }
        }
        printf("%s", body); 
    }

    if (received == -1)
    {
        perror("recv\n");
        exit(1);
    }
}

void iman_cmd(char *argument)
{
    struct hostent *server;
    struct sockaddr_in server_addr;
    int socket_fd;

    server = gethostbyname("man.he.net");
    if (server == NULL)
    {
        fprintf(stderr, "Error: No such host\n");
        exit(1);
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect");
        exit(1);
    }



    send_request(socket_fd, argument);


    filter(socket_fd);

    close(socket_fd);
}