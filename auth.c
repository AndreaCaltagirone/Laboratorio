#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

static const char base64_table[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_decode(const char *input, char *output, size_t output_size) {
    int val = 0, valb = -8;
    size_t out_len = 0;
    
    fprintf(stderr, "=== BASE64_DECODE ===\n");
    fprintf(stderr, "input: '%s'\n", input);
    
    for (const char *p = input; *p && *p != '='; p++) {
        const char *pos = strchr(base64_table, *p);
        if (pos == NULL) {
            fprintf(stderr, "Invalid base64 character: '%c' (0x%02x)\n", *p, (unsigned char)*p);
            return -1;
        }
        
        val = (val << 6) + (pos - base64_table);
        valb += 6;
        
        if (valb >= 0) {
            if (out_len >= output_size - 1) {
                fprintf(stderr, "Output buffer too small\n");
                return -1;
            }
            output[out_len++] = (char)((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    
    output[out_len] = '\0';
    fprintf(stderr, "Decoded: '%s' (length: %zu)\n", output, out_len);
    return (int)out_len;
}

int check_auth(const char *auth_header) {
    fprintf(stderr, "=== CHECK_AUTH ===\n");
    fprintf(stderr, "auth_header: '%s'\n", auth_header);
    
    const char *valid_user = "admin";
    const char *valid_password = "123";
    
    const char *basic = strstr(auth_header, "Basic ");
    fprintf(stderr, "basic pointer: %p\n", (void*)basic);
    
    if (basic == NULL) {
        fprintf(stderr, "Basic NOT found\n");
        return 0;
    }
    
    basic += 6;
    fprintf(stderr, "After +=6, basic: '%s'\n", basic);
    
    // Estrai solo la stringa base64 fino al newline o spazio
    char base64_str[256];
    int i = 0;
    while (basic[i] && basic[i] != '\r' && basic[i] != '\n' && basic[i] != ' ' && i < 255) {
        base64_str[i] = basic[i];
        i++;
    }
    base64_str[i] = '\0';
    
    fprintf(stderr, "base64_str extracted: '%s'\n", base64_str);
    fprintf(stderr, "base64_str length: %d\n", i);
    
    char decoded[256];
    int decode_result = base64_decode(base64_str, decoded, sizeof(decoded));
    fprintf(stderr, "base64_decode returned: %d\n", decode_result);
    
    if (decode_result < 0) {
        fprintf(stderr, "base64_decode failed\n");
        return 0;
    }
    
    fprintf(stderr, "decoded: '%s'\n", decoded);
    
    char *colon = strchr(decoded, ':');
    if (colon == NULL) {
        fprintf(stderr, "No colon found in decoded string\n");
        return 0;
    }
    
    *colon = '\0';
    char *username = decoded;
    char *password = colon + 1;
    
    fprintf(stderr, "username: '%s', password: '%s'\n", username, password);
    fprintf(stderr, "Comparing with valid_user: '%s', valid_password: '%s'\n", valid_user, valid_password);
    
    if (strcmp(username, valid_user) == 0 && strcmp(password, valid_password) == 0) {
        fprintf(stderr, "✓ AUTH SUCCESS\n");
        return 1;
    }
    
    fprintf(stderr, "✗ AUTH FAILED\n");
    return 0;
}