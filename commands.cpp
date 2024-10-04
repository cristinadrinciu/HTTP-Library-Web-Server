#include "commands.hpp"
#include "stdlib.h"

void register_user(int sockfd) {
    std :: string username, password;

    // read the username and password
    std :: cout << "username=";
    std :: getline(std :: cin, username);

    std :: cout << "password=";
    std :: getline(std :: cin, password);

    // check if the password is valid
    if(password.empty() || password.find(" ") != std :: string :: npos
        || username.empty() || username.find(" ") != std :: string :: npos) {
        std :: cout << "Invalid input\n";
        return;
    }

    // create the json object
    nlohmann :: json json_to_send;

    json_to_send["username"] = username;
    json_to_send["password"] = password;

    // create the message_body
    char **message_body = new char*[1];
    message_body[0] = new char[json_to_send.dump().size() + 1];
    memcpy(message_body[0], json_to_send.dump().c_str(), json_to_send.dump().size() + 1);

    // create the POST request, no cookies needed
    char *request = compute_post_request(SERVER_INFO, REGISTER, CONTENT_TYPE, message_body, 1, NULL, 0, NULL);

    // send the request to the server
    send_to_server(sockfd, request);

    // receive the response from the server
    char *response = receive_from_server(sockfd);

    // parse the response to check is there is an error
    char* extracted_response = basic_extract_json_response(response);
    if (extracted_response == NULL) {
        // there is no response, so no error
        std :: cout << "SUCCESS - 200: User " << username << " registered successfully\n";
    } else {
        // parse the response to check if there is an error
        nlohmann :: json json_response = nlohmann :: json :: parse(extracted_response);

        std :: cout << "ERROR: " << json_response["error"] << "\n";
    }

    // free the memory
    free(request);
    free(response);
    delete[] message_body[0];
    delete[] message_body;
}

void login_user(int sockfd, std :: string &received_cookie) {
    std :: string username, password;

    // read the username
    std :: cout << "username=";
    std :: getline(std :: cin, username);

    // read the password
    std :: cout << "password=";
    std :: getline(std :: cin, password);

    // check if the password is valid
    if(password.empty() || password.find(" ") != std :: string :: npos
        || username.empty() || username.find(" ") != std :: string :: npos) {
        std :: cout << "Invalid input\n";
        return;
    }

    // create the json object
    nlohmann :: json json_to_send;

    json_to_send["username"] = username;
    json_to_send["password"] = password;

    // create the message_body
    char **message_body = new char*[1];
    message_body[0] = new char[json_to_send.dump().size() + 1];
    memcpy(message_body[0], json_to_send.dump().c_str(), json_to_send.dump().size() + 1);

    // create the POST request, no cookies needed
    char *request = compute_post_request(SERVER_INFO, LOGIN, CONTENT_TYPE, message_body, 1, NULL, 0, NULL);

    // send the request to the server
    send_to_server(sockfd, request);

    // receive the response from the server
    char *response = receive_from_server(sockfd);

    // parse the response
    char* extracted_response = basic_extract_json_response(response);

    if (extracted_response == NULL) {
        // extract the cookie
        // first, set the current cookie to empty
        received_cookie = "";

        // extract the cookie
        // it is marked by "connect.sid=" and ends with ";" or "\n"
        received_cookie = strtok(strstr(response, "connect.sid"), ";\n");

        std :: cout << "SUCCESS - 200: User " << username << " logged in successfully\n";
    } else {
        // parse the response to check if there is an error
        nlohmann :: json json_response = nlohmann :: json :: parse(extracted_response);

        std :: cout << "ERROR: " << json_response["error"] << "\n";
    }

    // free the memory
    free(request);
    free(response);
    delete[] message_body[0];
    delete[] message_body;
}

void enter_library(int sockfd, std::string received_cookie, std::string &jwt_token) {
    // check if the user is logged in
    if (received_cookie == "") {
        std::cout << "ERROR: You are not logged in\n";
        return;
    }

    // create a vector of cookies
    char **cookies = new char*[1];
    cookies[0] = new char[received_cookie.size() + 1];
    strcpy(cookies[0], received_cookie.c_str());

    // create the GET request
    char *request = compute_get_request(SERVER_INFO, LIB_ACCESS, NULL, cookies, 1, NULL);

    // send the request to the server
    send_to_server(sockfd, request);

    // receive the response from the server
    char *response = receive_from_server(sockfd);

    // parse the response
    char* extracted_response = basic_extract_json_response(response);

    // check if the token was received
    if (strstr(response, "token") != NULL) {
        // parse the response to get the token
        nlohmann::json json_response = nlohmann::json::parse(extracted_response);
        jwt_token = json_response["token"];
        std::cout << "SUCCESS - 200: You have entered the library successfully\n";
    } else {
        // parse the response to check if there is an error
        nlohmann::json json_response = nlohmann::json::parse(extracted_response);
        std::cout << "ERROR: " << json_response["error"] << "\n";
    }

    // free the memory
    free(request);
    free(response);
    delete[] cookies[0];
    delete[] cookies;
}

void get_books(int sockfd, std::string jwt_token) {
    // check if the user is logged in
    if (jwt_token == "") {
        std::cout << "ERROR: You did not access the library\n";
        return;
    }

    // create the GET request
    char *request = compute_get_request(SERVER_INFO, BOOKS, NULL, NULL, 0, jwt_token.c_str());

    // send the request to the server
    send_to_server(sockfd, request);

    // receive the response from the server
    char *response = receive_from_server(sockfd);

    // parse the response
    char* extracted_response = basic_extract_json_response(response);

    // check if the books were received
    if (strstr(response, "[")) {
        // it means that the books were received, an array of books
        nlohmann::json json_response = nlohmann::json::parse(strstr(response, "["));
        std :: cout << json_response.dump(4) << "\n";
    } else {
        // parse the response to check if there is an error
        nlohmann :: json json_error_response = nlohmann :: json :: parse(extracted_response);

        std :: cout << "ERROR: " << json_error_response["error"] << "\n";
    }

    // free the memory
    free(request);
    free(response);
}

void add_book(int sockfd, std :: string jwt_token) {
    // check if the user accessed the library
    if (jwt_token == "") {
        std :: cout << "ERROR: You did not access the library\n";
        return;
    }

    std :: string title, author, genre, page_count, publisher;
    std :: cout << "title=";
    std :: getline(std :: cin, title);

    std :: cout << "author=";
    std :: getline(std :: cin, author);

    std :: cout << "genre=";
    std :: getline(std :: cin, genre);

    std :: cout << "page_count=";
    std :: getline(std :: cin, page_count);

    std :: cout << "publisher=";
    std :: getline(std :: cin, publisher);

    // check if the page_count is a number
    if (page_count.find_first_not_of("0123456789") != std :: string :: npos) {
        std :: cout << "ERROR: Invalid page_count\n";
        return;
    }

    // check if the title, author, genre, publisher are not empty
    if (title.empty() || author.empty() || genre.empty() || publisher.empty()) {
        std :: cout << "ERROR: Invalid title, author, genre or publisher\n";
        return;
    }

    // create the json object
    nlohmann :: json json_to_send;

    json_to_send["title"] = title;
    json_to_send["author"] = author;
    json_to_send["genre"] = genre;
    json_to_send["publisher"] = publisher;
    json_to_send["page_count"] = std :: stoi(page_count);
    
    // create the message_body
    char **message_body = new char*[1];
    message_body[0] = new char[json_to_send.dump().size() + 1];
    memcpy(message_body[0], json_to_send.dump().c_str(), json_to_send.dump().size() + 1);

    // create the POST request
    char *request = compute_post_request(SERVER_INFO, BOOKS, CONTENT_TYPE, message_body, 1, NULL, 0, jwt_token.c_str());

    // send the request to the server
    send_to_server(sockfd, request);

    // receive the response from the server
    char *response = receive_from_server(sockfd);

    // parse the response
    char* extracted_response = basic_extract_json_response(response);

    if (extracted_response == NULL) {
        std :: cout << "SUCCESS - 200: Book " << title << " added successfully\n";
    } else {
        // parse the response to check if there is an error
        nlohmann :: json json_response = nlohmann :: json :: parse(extracted_response);

        std :: cout << "ERROR: " << json_response["error"] << "\n";
    }

    // free the memory
    free(request);
    free(response);
    delete[] message_body[0];
    delete[] message_body;
}

void get_book(int sockfd, std :: string jwt_token) {
    // check if the user accessed the library
    if (jwt_token == "") {
        std :: cout << "ERROR: You did not access the library\n";
        return;
    }

    std :: string id;
    std :: cout << "id=";
    std :: getline(std :: cin, id);

    // check if the id is a number
    if (id.find_first_not_of("0123456789") != std :: string :: npos) {
        std :: cout << "ERROR: Invalid id\n";
        return;
    }

    // build the path to the book
    std :: string path = BOOKS;
    path += "/";
    path += id;

    // create the GET request
    char *request = compute_get_request(SERVER_INFO, path.c_str(), id.c_str(), NULL, 0, jwt_token.c_str());

    // send the request to the server
    send_to_server(sockfd, request);

    // receive the response from the server
    char *response = receive_from_server(sockfd);

    // parse the response
    char* extracted_response = basic_extract_json_response(response);

    // check if the book was received
    if (strstr(response, "title")) {
        // it means that the book was received
        nlohmann::json json_response = nlohmann::json::parse(extracted_response);

        // print the details of the book
        std :: cout << json_response.dump(4) << "\n";
    } else {
        // parse the response to check if there is an error
        nlohmann :: json json_error_response = nlohmann :: json :: parse(extracted_response);

        std :: cout << "ERROR: " << json_error_response["error"] << "\n";
    }

    // free the memory
    free(request);
    free(response);
}

void delete_book(int sockfd, std :: string jwt_token) {
    // check if the user accessed the library
    if (jwt_token == "") {
        std :: cout << "ERROR: You did not access the library\n";
        return;
    }

    std :: string id;
    std :: cout << "id=";
    std :: getline(std :: cin, id);

    // check if the id is a number
    if (id.find_first_not_of("0123456789") != std :: string :: npos) {
        std :: cout << "ERROR: Invalid id\n";
        return;
    }

    // build the path to the book
    std :: string path = BOOKS;
    path += "/";
    path += id;

    // create the DELETE request
    char *request = compute_delete_request(SERVER_INFO, path.c_str(), NULL, 0, jwt_token.c_str());

    // send the request to the server
    send_to_server(sockfd, request);

    // receive the response from the server
    char *response = receive_from_server(sockfd);

    // parse the response
    char* extracted_response = basic_extract_json_response(response);

    if (extracted_response == NULL) {
        std :: cout << "SUCCESS - 200: Book with id " << id << " deleted successfully\n";
    } else {
        // parse the response to check if there is an error
        nlohmann :: json json_response = nlohmann :: json :: parse(extracted_response);

        std :: cout << "ERROR: " << json_response["error"] << "\n";
    }

    // free the memory
    free(request);
    free(response);
}

void logout_user(int sockfd, std :: string &received_cookie, std :: string &jwt_token) {
    // check if the user is logged in
    if (received_cookie == "") {
        std :: cout << "ERROR: You are not logged in\n";
        return;
    }

    // create a vector of cookies
    char **cookies = new char*[1];
    cookies[0] = new char[received_cookie.size() + 1];
    strcpy(cookies[0], received_cookie.c_str());

    // create the GET request
    char *request = compute_get_request(SERVER_INFO, LOGOUT, NULL, cookies, 1, jwt_token.c_str());

    // send the request to the server
    send_to_server(sockfd, request);

    // receive the response from the server
    char *response = receive_from_server(sockfd);

    // parse the response
    char* extracted_response = basic_extract_json_response(response);

    if (extracted_response == NULL) {
        std :: cout << "SUCCESS - 200: User logged out successfully\n";
    } else {
        // parse the response to check if there is an error
        nlohmann :: json json_response = nlohmann :: json :: parse(extracted_response);

        std :: cout << "ERROR: " << json_response["error"] << "\n";
    }

    // free the memory
    free(request);
    free(response);
    delete[] cookies[0];
    delete[] cookies;

    // reset the cookies and jwt_token
    received_cookie = "";
    jwt_token = "";
}