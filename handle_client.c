#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <regex.h>

#include "header.h"

#define BUFFER_SIZE 4096
#define PORT 8080

void *handle_client(void *arg) {

    int client_fd = *(int *)arg;
    free (arg);

    char buffer[BUFFER_SIZE] = {0};

    regmatch_t matches[3];

    char method[16] = {0};
    char path[256] = {0};

    ssize_t byte_rcv = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (byte_rcv <= 0)
    {
        close(client_fd);
        return NULL;
    }

    buffer[byte_rcv] = '\0';

    if (regexec(&re_http, buffer, 3, matches, 0) == 0) {

        //metodo
        int len = matches[1].rm_eo - matches[1].rm_so;
        strncpy(method, buffer + matches[1].rm_so, len);
        method[len] = '\0';

        //path
        len = matches[2].rm_eo - matches[2].rm_so;
        strncpy(path, buffer + matches[2].rm_so, len);
        path[len] = '\0';

        if (strstr(path, "admin") != NULL)
        {
            char *auth_line = strstr(buffer, "Authorization:");

            if(auth_line == NULL || !check_auth(auth_line)) {
                send_response(client_fd, 401, "text/plain", "Unauthorized\n", 13);
                close(client_fd);
                return NULL;
            }
        }
        

        if ((strcmp(method, "GET")) == 0)
        {
            handle_get(client_fd, path);
                
        }else if ((strcmp(method, "DELETE")) == 0)
        {
            handle_delete(client_fd, path);

        }else if ((strcmp(method, "PUT")) == 0)
        {
            char *body = strstr(buffer, "\r\n\r\n");
            if (body != NULL) {
                body += 4; // Salta "\r\n\r\n"
            }
            handle_put(client_fd, path, body);

        }else if ((strcmp(method, "POST")) == 0)
        {
            char *body = strstr(buffer, "\r\n\r\n");
            if (body != NULL) {
                body += 4; // Salta "\r\n\r\n"
            }
            handle_post(client_fd, path, body);

        }else{
            send_response(client_fd, 400, "text/plain", "Bad Request: Metodo non supportato", 34);
        }
    }else{
        send_response(client_fd, 400, "text/plain", "Bad Request: Formato richiesta invalido", 40);
    }

    close(client_fd);
    return NULL;

}
    