#include <stdio.h>
#include <string.h>
#include <unistd.h> 

#include "header.h"

char* get_extension(const char *path) {
    char *ext = strrchr(path, '.');
    return (ext == NULL) ? "" : ext + 1;
}

char* get_mime_type(const char *ext) {
    if (strcmp(ext, "html") == 0) return "text/html";
    if (strcmp(ext, "css") == 0)  return "text/css";
    if (strcmp(ext, "js") == 0)   return "application/javascript";
    if (strcmp(ext, "txt") == 0)  return "text/plain";
    if (strcmp(ext, "json") == 0) return "application/json";
    if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, "png") == 0)  return "image/png";
    return "application/octet-stream";
}

void send_response(int client_fd, int status_code, const char *content_type,
                   const char *body, size_t body_len) {
    char header[512];
    const char *status_text;

    // Determina il testo dello status
    switch (status_code) {
        case 200: status_text = "OK"; break;
        case 201: status_text = "Created"; break;
        case 204: status_text = "No Content"; break;
        case 400: status_text = "Bad Request"; break;
        case 401: status_text = "Unauthorized"; break;
        case 404: status_text = "Not Found"; break;
        case 500: status_text = "Internal Server Error"; break;
        default: status_text = "Unknown"; break;
    }

    // Costruisci header
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "%s"
        "Connection: close\r\n"
        "\r\n",
        status_code, status_text, content_type, body_len, (status_code == 401 ? "WWW-Authenticate: Basic realm=\"Protected Area\"\r\n" : ""));

    // Invia header
    write(client_fd, header, header_len);
    
    // Invia body (se presente)
    if (body_len > 0 && body != NULL) {
        write(client_fd, body, body_len);
    }
}