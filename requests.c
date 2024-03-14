#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "requests.h"
#include "helpers.h"

// from lab with little modifications to the header
char *compute_post_request(char *host, char *url, char *content_type, char *body_data,
                           char **cookies, int cookies_count, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* add necessary headers (Content-Type and Content-Length are mandatory)
       in order to write Content-Length,first compute the message size */
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    int size = strlen(body_data);

    sprintf(line, "Content-Length:%d", size);
    compute_message(message, line);

    // add cookies (optional)
    if (cookies != NULL) {
        for (int i = 0; i < cookies_count; i++) {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }

    // add authorization (optional)
    if (jwt != NULL) {
        sprintf(line, "Authorization: %s", jwt);
        compute_message(message, line);
    }

    // add new line at end of header
    compute_message(message, "");
    // add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data);

    free(line);
    return message;
}

// from lab with little modifications to the header
char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *jwt) {

    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // add the host
    char *hostHeader = calloc(LINELEN, sizeof(char));
    sprintf(hostHeader, "Host: %s", host);
    compute_message(message, hostHeader);
    // (optional) add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        for (int i = 0; i < cookies_count; i++) {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }

    // authorization with jwt
    if(jwt != NULL) {
        sprintf(line, "Authorization: %s", jwt);
        compute_message(message, line);
    }
    // add final new line
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char **cookies, int cookies_count, char *jwt) {

    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // write the method name, URL and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // add the host
    char *hostHeader = calloc(LINELEN, sizeof(char));
    sprintf(hostHeader, "Host: %s", host);
    compute_message(message, hostHeader);
    // (optional) add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        for (int i = 0; i < cookies_count; i++) {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }

    // authorization with jwt
    if (jwt != NULL) {
        sprintf(line, "Authorization: %s", jwt);
        compute_message(message, line);
    }
    
    // add final new line
    compute_message(message, "");
    free(line);

    return message;
}