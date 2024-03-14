#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "functions.h"
#define HOST "34.254.242.81"
#define PORT 8080
#define BUFFER 1000

void registration(char *username, char *password) {
    char *message;
    char *response;
    int sockfd;

    // connect to server
    sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);
    // serialize into a json char*
    char *payload_json = serialization_register_login(username, password);
    // construct the POST message
    message = compute_post_request(HOST, "/api/v1/tema/auth/register", "application/json", payload_json, NULL, 0, NULL);
    // send to server
    send_to_server(sockfd, message);
    // retrieve reponse from server
    response = receive_from_server(sockfd);
    // in case the username is already used or not
    char *pointer = strstr(response, "Bad Request");
    if (pointer != NULL) {
        printf("Error! The username %s is already used!\n", username);
    } else {
        printf("User added successfully!\n");
    }
    // close communication
    close(sockfd);
}

char* login(char* username, char* password) {
    char *message;
    char *response;
    char *cookie = malloc(BUFFER);
    int sockfd;

    // connect to server
    sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);
    // serialize into a json char*
    char *payload_json = serialization_register_login(username, password);
    // construct the POST message
    message = compute_post_request(HOST, "/api/v1/tema/auth/login", "application/json", payload_json, NULL, 0, NULL);
    // send to server
    send_to_server(sockfd, message);
    // retrieve reponse from server
    response = receive_from_server(sockfd);
    // in case the credentials are not good
    char *pointer = strstr(response, "Bad Request");
    if (pointer != NULL) {
        printf("Error! Credentials are not good!\n");
        return NULL;
    } else {
        // retrieve the cookie that demonstrates we are logged in
        char *pointer1 = strstr(response, "Set-Cookie:");

        if(pointer1 != NULL) {
            pointer1 = strtok(pointer1, "\n");
            int sub_elim = strlen("Set-Cookie: ");
            memmove(cookie, pointer1 + sub_elim, strlen(pointer1) - sub_elim);
            cookie = strtok(cookie, " ");
            printf("Login success!\n");
        }
    }

    json_free_serialized_string(payload_json);
    close(sockfd);
    return cookie;
}

char* enter(char *cookie) {
    char *message;
    char *response;
    char *jwt = malloc(BUFFER);
    int sockfd;

    // connect to server
    sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);
    // construct the GET message
    message = compute_get_request(HOST, "/api/v1/tema/library/access", NULL, &cookie, 1, NULL);
    // send to server
    send_to_server(sockfd, message);
    // retrieve reponse from server
    response = receive_from_server(sockfd);
    
    // check if we are logged in
    char *pointer1 = strstr(response, "Unauthorized");
    if(pointer1 != NULL) {
        printf("Error! You are not logged in!\n");
        close(sockfd);
        return NULL;
    }

    // get the jwt token and add the Bearer string to it
    pointer1 = basic_extract_json_response(response);
     if (pointer1 != NULL)
     {
         int sub_elim = strlen("{\"token\":\"");
         memmove(jwt, pointer1 + sub_elim, strlen(pointer1) - sub_elim);
         jwt = strtok(jwt, "\"");

         char Bearer[BUFFER] = "Bearer ";
         strcat(Bearer, jwt);
         strcpy(jwt, Bearer);

         printf("You've entered the library successfully!\n");
     }
    
    // disconnect from server
     close(sockfd);
     return jwt;
}

void get_books(char *jwt, char *cookie) {
    char *message;
    char *response;
    int sockfd;

    // connect to server
    sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);
    // construct the GET message
    message = compute_get_request(HOST, "/api/v1/tema/library/books", NULL, &cookie, 1, jwt);
    // send to server
    send_to_server(sockfd, message);
    // retrieve reponse from server
    response = receive_from_server(sockfd);

    // check if we have access to the library
    char *pointer1 = strstr(response, "Forbidden");
    if(pointer1 != NULL) {
        printf("Error! You do not have access to the library!\n");
        close(sockfd);
        return;
    }
    // check if the book library is empty
    pointer1 = extract_json_responses(response);

    if (strcmp(pointer1, "[]") != 0) {
        show_multiple_books(pointer1);
    }
    else if (strcmp(pointer1, "[]") == 0) {
        printf("The list is empty. Fill it with books!\n");
    }

    close(sockfd);
}

void get_book(int id, char *jwt, char *cookie)
{
    char *message;
    char *response;
    int sockfd;

    // connect to server
    sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);
    // add the id in the url
    char *line = calloc(LINELEN, sizeof(char));
    sprintf(line, "/api/v1/tema/library/books/%d", id);
    // construct the get message
    message = compute_get_request(HOST, line, NULL, &cookie, 1, jwt);
    // send to server
    send_to_server(sockfd, message);
    // retrieve reponse from server
    response = receive_from_server(sockfd);

    // check if we have authorization
    char *pointer = strstr(response, "Forbidden");
    if(pointer != NULL) {
        printf("Error! You do not have access to the library!\n");
        free(line);
        close(sockfd);
        return;
    }

    // we should parse the response or treat the error case in case the id is inexistent
    pointer = strstr(response, "error");
    if (pointer != NULL) {
        printf("Error! Book with id=%d was not found!\n", id);
        return;
    } else {
        char *pointer1 = basic_extract_json_response(response);
        // to eliminate the ] character
        pointer1[strlen(pointer1)] = '\0';
        show_one_book(pointer1);
    }

    // disconnect
    free(line);
    close(sockfd);
}

void add_book(char *title, char *author, char *genre, char *publisher, int page_count, char *jwt, char *cookie)
{
    char *message;
    char *response;
    int sockfd;

    // connect to server
    sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);
    // serialize into a json
    char *payload_json = serialization_add(title, author, genre, publisher, page_count);
    // construct the GET message
    message = compute_post_request(HOST, "/api/v1/tema/library/books", "application/json", payload_json, &cookie, 1, jwt);
    // send
    send_to_server(sockfd, message);
    // receive
    response = receive_from_server(sockfd);
    // show if we added the book
    char *pointer = strstr(response, "OK");
    if (pointer != NULL) {
        printf("The book %s has been added successfully\n", title);
    } else {
        printf("Invalid input!\n");
    }

    close(sockfd);
}

void delete_book(int id, char *jwt, char *cookie) {
    char *message;
    char *response;
    int sockfd;

    // connect to server
    sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);

    // add the id to the url
    char *line = calloc(LINELEN, sizeof(char));
    sprintf(line, "/api/v1/tema/library/books/%d", id);
    // construct the DELETE message
    message = compute_delete_request(HOST, line, &cookie, 1, jwt);
    // send
    send_to_server(sockfd, message);
    // receive response
    response = receive_from_server(sockfd);

    // check if we have access
    char *pointer = strstr(response, "Forbidden");
    if (pointer != NULL)
    {
        printf("Error! You do not have access to the library!\n");
        free(line);
        close(sockfd);
        return;
    }

    // check if the book exists else it gets deleted
    pointer = strstr(response, "Not Found");
    if (pointer != NULL) {
        printf("Error! Invalid id!\n");
    }
    else if ((pointer = strstr(response, "OK")) != NULL) {

        printf("Book deleted successfully!\n");
    }

    // disconnect
    free(line);
    close(sockfd);
}

void logout(char **cookie, char *jwt) {
    char *message;
    char *response;
    int sockfd;

    // open connection
    sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);
    // construct GET message
    message = compute_get_request(HOST, "/api/v1/tema/auth/logout", NULL, cookie, 1, jwt);
    // send and receive response
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // in case we were not logged in
    char *pointer = strstr(response, "Bad Request");
    if (pointer != NULL) {
        printf("Error! You are not logged in!\n");
    } else {
        printf("Logged out!\n");
        free(jwt);
        free(*cookie);
    }
    close(sockfd);
}

void free_tokens(char *cookie, char *token) {
    free(cookie);
    free(token);
}
