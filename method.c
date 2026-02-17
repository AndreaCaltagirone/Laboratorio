#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>

#include "header.h"

#define BUFFER_SIZE 4096
#define WEB_ROOT "./www/"

void handle_get(int client_fd, const char *path){

    char file_path[512];
    int file_fd;
    snprintf(file_path, sizeof(file_path), "%s%s", WEB_ROOT, path);

    char *ext =get_extension(path);
    char *mime_type =get_mime_type(ext);

    file_fd = open(file_path, O_RDONLY);
    if (file_fd == -1){
        send_response(client_fd, 404, "text/plain", "404 Not Found\n", 14);
        return;
    }
    
    struct stat file_stat;
    fstat(file_fd, &file_stat);
    off_t file_size = file_stat.st_size;

    char *file_content = malloc(file_size);
    if ((file_content == NULL))
    {
        close(file_fd);
        send_response(client_fd, 500, "text/plain", "Internal Server Error\n", 22);
        return;
    }

    ssize_t bytes_read = read(file_fd, file_content, file_size);
    close(file_fd);

    if (bytes_read != file_size) {
        free(file_content);
        send_response(client_fd, 500, "text/plain", "Internal Server Error\n", 22);
        return;
    }

    send_response(client_fd, 200, mime_type, file_content, file_size);

    free(file_content);

}

void handle_post(int client_fd, const char *path, const char *body){

    char file_path[512];

    snprintf(file_path, sizeof(file_path), "%s%s", WEB_ROOT, path);

    int file_fd = open(file_path, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (file_fd == -1) {
        send_response(client_fd, 400, "text/plain", "Bad Request\n", 12);
        return;
    }

    if (body != NULL) {
        write(file_fd, body, strlen(body));
    }
    close(file_fd);

    send_response(client_fd, 201, "text/plain", "Created\n", 8);
}

void handle_put(int client_fd, const char *path, const char *body) {

    char file_path[512];

    snprintf(file_path, sizeof(file_path), "%s%s", WEB_ROOT, path);

    int file_fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd == -1) {
        send_response(client_fd, 400, "text/plain", "Bad Request", 11);
        return;
    }

    if (body != NULL) {
        write(file_fd, body, strlen(body));
    }
    close(file_fd);

    send_response(client_fd, 204, "text/plain", "", 0);
}

void handle_delete(int client_fd, const char *path) {

    char file_path[512];

    snprintf(file_path, sizeof(file_path), "%s%s", WEB_ROOT, path);

    if (unlink(file_path) == -1) {
        send_response(client_fd, 404, "text/plain", "Not Found", 9);
        return;
    }

    send_response(client_fd, 204, "text/plain", "", 0);
}
