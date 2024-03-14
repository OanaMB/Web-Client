#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "parson.h"
#include "helpers.h"
#include "buffer.h"

// from lab
void compute_message(char *message, const char *line) {
    strcat(message, line);
    strcat(message, "\r\n");
}

// from lab
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag) {
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        perror("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        perror("ERROR connecting");

    return sockfd;
}

/* function that takes char* and serializes them into a json type, using functions from parson */
char* serialization_register_login(char *username, char* password) {
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", username);
    json_object_dotset_string(root_object, "password", password);
    serialized_string = json_serialize_to_string_pretty(root_value);
    json_value_free(root_value);
    return serialized_string;
}

/* function that takes char* and serializes them into a json type, using functions from parson */
char *serialization_add(char *title, char *author, char *genre, char *publisher, int page_count) {
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_number(root_object, "page_count", page_count);
    json_object_set_string(root_object, "publisher", publisher);
    serialized_string = json_serialize_to_string_pretty(root_value);
    json_value_free(root_value);
    return serialized_string;
}


// send to server function 
void send_to_server(int sockfd, char *message) {
    int bytes, sent = 0;
    int total = strlen(message);

    do {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            perror("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

// receive from server function
char *receive_from_server(int sockfd) {
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            perror("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t)bytes);

        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;

            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);

            if (content_length_start < 0) {
                continue;
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);

    size_t total = content_length + (size_t)header_end;

    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            perror("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t)bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

// checks for one json element
char *basic_extract_json_response(char *str) { return strstr(str, "{\""); }
// checks for an array of json elements
char *extract_json_responses(char *str) { return strstr(str, "["); }
// function that shows multiple json elements gotten from the server using parson; used for only showing id and title
void show_multiple_books(char *elems)
{
    // create an array out of the json char*
    JSON_Value *value = json_parse_string(elems);
    JSON_Array *show = json_value_get_array(value);

    // go through the array and print the values in the fields
    for (size_t i = 0; i < json_array_get_count(show); i++) {
        JSON_Object *element = json_array_get_object(show, i);
        printf("\nid=%d\ntitle=%s\n",
        (int)json_object_get_number(element, "id"), json_object_get_string(element, "title"));
    }

    json_value_free(value);
}

// function that shows one json element gotten from the server using parson; used for showing all the details of a book
void show_one_book(char *jsonelement) {
    // create an object
    JSON_Value *value = json_parse_string(jsonelement);
    JSON_Object *show = json_value_get_object(value);
    // print the values in the object
    printf("\nid=%d\ntitle=%s\nauthor=%s\npublisher=%s\ngenre=%s\npage_count=%d\n",
        (int)json_object_get_number(show, "id"), json_object_get_string(show, "title"),
        json_object_get_string(show, "author"), json_object_get_string(show, "publisher"), 
        json_object_get_string(show, "genre"), (int)json_object_get_number(show, "page_count"));
    json_value_free(value);
}

int isNumeric(const char *str)
{
    char *end;
    strtol(str, &end, 10);
    return *end == '\0';
}