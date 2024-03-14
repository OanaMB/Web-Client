#include "functions.h"
#include "parson.h"
#include "helpers.h"
#include "requests.h"

#define SIZE_COMMAND 1000

int main(int argc, char *argv[]) {

    char command[SIZE_COMMAND];
    char *cookie = NULL;
    char *jwt = NULL;

    while(1) {
        // we read the comand from stdin
        scanf("%s", command);

        // trat the case when we want to register an account on the server
        if(strcmp(command, "register") == 0) {
            char username[SIZE_COMMAND];
            char password[SIZE_COMMAND];

            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            registration(username, password);
        }

        // in case we want to login
        if (strcmp(command, "login") == 0) {
            // functie login
            char username[SIZE_COMMAND];
            char password[SIZE_COMMAND];
            char temp;

            printf("username=");
            scanf("%c", &temp);
            scanf("%[^\n]", username);
            printf("password=");
            scanf("%c", &temp);
            scanf("%[^\n]", password);

            // in case we were logged before
            if(cookie != NULL) {
                free_tokens(cookie, jwt);
                cookie = NULL;
                jwt = NULL;
            }

            cookie = login(username, password);
        }

        // in case we want access to the library
        if (strcmp(command, "enter_library") == 0) {
            jwt = enter(cookie);
        }

        // in case we want to see all the books that we have in our library
        if (strcmp(command, "get_books") == 0) {
            get_books(jwt, cookie);
        }

        // in case we want details of one book
        if (strcmp(command, "get_book") == 0) {
            int id;
            printf("id=");
            scanf("%d", &id);
            get_book(id, jwt, cookie);
        }

        // in case we want to add a book to our library
        if (strcmp(command, "add_book") == 0) {
          
            char title[SIZE_COMMAND], author[SIZE_COMMAND], genre[SIZE_COMMAND], publisher[SIZE_COMMAND];
            char temp;
            int page_count;

            // check if we have authorization
            if (jwt == NULL) {
                printf("Error! You do not have access to the library!\n");
            } else {
                // we read the data from a book
            
                scanf("%c", &temp);
                printf("title=");
                fgets(title, SIZE_COMMAND, stdin);
                printf("author=");
                fgets(author, SIZE_COMMAND, stdin);
                printf("genre=");
                fgets(genre, SIZE_COMMAND, stdin);
                printf("publisher=");
                fgets(publisher, SIZE_COMMAND, stdin);
                printf("page_count=");

                // checking if the page_count is valid
                char input[SIZE_COMMAND];
                fgets(input, SIZE_COMMAND, stdin);
                input[strcspn(input, "\n")] = '\0';

                if (!isNumeric(input) || strcmp(input, "") == 0) {
                    printf("Invalid input!\n");
                    continue;
                }

                page_count = atoi(input);

                // eliminate the new line that fgets puts and checking if the input is valid
                author[strlen(author) - 1] = '\0';
                title[strlen(title) - 1] = '\0';
                genre[strlen(genre) - 1] = '\0';
                publisher[strlen(publisher) - 1] = '\0';
               
                if(strcmp(title,"") == 0 || strcmp(author, "") == 0 || strcmp(genre, "") == 0 || strcmp(publisher,"") == 0) {
                    printf("Invalid input!\n");
                    continue;
                }

                add_book(title, author, genre, publisher, page_count, jwt, cookie);
            }
        }

        // in case we want to delete a book
        if (strcmp(command, "delete_book") == 0) {
            int id;
            printf("id=");
            scanf("%d", &id);
            delete_book(id, jwt, cookie);
        }

        // in case we want to logout
        if (strcmp(command, "logout") == 0) {
            logout(&cookie, jwt);
            if(cookie != NULL || jwt != NULL) {
            cookie = NULL;
            jwt = NULL;
            }
        }

        // exit the program
        if (strcmp(command, "exit") == 0) {
            free_tokens(cookie, jwt);
            jwt = NULL;
            cookie = NULL;
            break;
        }

        // we have bot provided a right command
        if (strcmp(command, "register") != 0 && strcmp(command, "login") != 0 && strcmp(command, "enter_library") != 0 &&
            strcmp(command, "get_books") != 0 && strcmp(command, "get_book") != 0 && strcmp(command, "add_book") != 0 &&
            strcmp(command, "delete_book") != 0 && strcmp(command, "logout") != 0 && strcmp(command, "exit") != 0)
            printf("Wrong command!\n");
    }

    return 0;
}