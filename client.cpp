#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <string>
#include "helpers.hpp"
#include "requests.hpp"
#include "iostream"
#include "commands.hpp"

int main(int argc, char *argv[])
{
    int sockfd;

    std :: string received_cookie = "";   
    std :: string jwt_token = "";

    std :: string input_command(LINELEN, '\0');

    while(1) {
        // firstly, connect to the server
        sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

        // read the command
        std :: getline(std :: cin, input_command);

        // check if it is a valid command, no whitespaces allowed
        if(input_command.find(" ") != std :: string :: npos) {
            std :: cout << "Invalid command\n";
            continue;
        }

        if (input_command == "register") {
            register_user(sockfd);
        } else if (input_command == "login") {
            login_user(sockfd, received_cookie);
        } else if (input_command == "enter_library") {
            enter_library(sockfd, received_cookie, jwt_token);
        } else if (input_command == "get_books") {
            get_books(sockfd, jwt_token);
        } else if (input_command == "get_book") {
            get_book(sockfd, jwt_token);
        } else if (input_command == "add_book") {
            add_book(sockfd, jwt_token);
        } else if (input_command == "delete_book") {
            delete_book(sockfd, jwt_token);
        } else if (input_command == "logout") {
            logout_user(sockfd, received_cookie, jwt_token);
        } else if (input_command == "exit") {
            break;
        } else {
            std :: cout << "Invalid command\n";
        }

        // close the connection
        close_connection(sockfd);
    }

    // close the connection
    close_connection(sockfd);

    return 0;
}
