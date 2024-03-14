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
#include "parson.h"

void registration(char *username, char *password);
char* login(char *username, char *password);
char* enter(char *cookie);
void get_books(char *jwt, char *cookie);
void get_book(int id, char *jwt, char *cookie);
void add_book(char *title, char *author, char *genre, char *publisher, int page_count, char *jwt, char *cookie);
void delete_book(int id, char *jwt, char *cookie);
void logout(char **cookie, char *jwt);
void free_tokens(char *cookie, char *token);