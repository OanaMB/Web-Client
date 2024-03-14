#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>

#define BUFLEN 4096
#define LINELEN 1000

char *compute_post_request(char *host, char *url, char *content_type, char *body_data,
                           char **cookies, int cookies_count, char *jwt);
char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *jwt);
char *compute_delete_request(char *host, char *url,char **cookies,int cookies_count,char *jwt);
