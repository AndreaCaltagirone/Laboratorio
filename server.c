#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "header.h"

#define BUFFER_SIZE 4096
#define PORT 8080
#define BACKLOG 10

regex_t re_http;

int main() {
    int server_fd, *new_sock;
    struct  sockaddr_in address;
    int addrlen = sizeof(address);

    pthread_t thread_id;

    regcomp(&re_http, "^(GET|POST|PUT|DELETE) /([^ ]*) HTTP/1", REG_EXTENDED);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket_failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0 )
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, BACKLOG) < 0)
    {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        new_sock = malloc(sizeof(int));

        if ((*new_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) >= 0)
        {
            if (pthread_create(&thread_id, NULL, handle_client, (void*)new_sock) != 0)
            {
                perror("pthread_create failed");
                close(*new_sock);
                free(new_sock);
                continue;
            }
            pthread_detach(thread_id);
        }else{
            free(new_sock);
        }
    }
}