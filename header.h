#ifndef HEADER_H
#define HEADER_H


#include <regex.h>

extern regex_t re_http;

void *handle_client(void *arg);

char* get_extension(const char *path); 
char* get_mime_type(const char* ext);
void send_response(int client_fd, int status_code, const char *content_type,const char *body, size_t body_len);

int check_auth(const char *auth_header);

void handle_get(int client_fd, const char *file_name);
void handle_post(int client_fd, const char *path, const char *body);
void handle_put(int client_fd, const char *path, const char *body);
void handle_delete(int client_fd, const char *path);

#endif