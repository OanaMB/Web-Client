#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "parson.h"

#define BUFLEN 4096
#define LINELEN 1000
#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

void compute_message(char *message, const char *line);
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);
char *serialization_register_login(char *username, char *password);
char *serialization_add(char *title, char *author, char *genre, char *publisher, int page_count);
void send_to_server(int sockfd, char *message);
char *receive_from_server(int sockfd);
char *basic_extract_json_response(char *str);
char *extract_json_responses(char *str);
void show_one_book(char *jsonelement);
void show_multiple_books(char *elems);
int isNumeric(const char *str);