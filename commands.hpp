#ifndef __COMMANDS__HPP__
#define __COMMANDS__HPP__

#include <string>
#include <vector>
#include "buffer.hpp"
#include "requests.hpp"
#include "nlohmann/json.hpp"
#include "helpers.hpp"
#include <iostream>

void register_user(int sockfd);
void login_user(int sockfd, std :: string &received_cookie);
void enter_library(int sockfd, std :: string received_cookie, std :: string &jwt_token);
void get_books(int sockfd, std :: string jwt_token);
void get_book(int sockfd, std :: string jwt_token);
void add_book(int sockfd, std :: string jwt_token);
void delete_book(int sockfd, std :: string jwt_token);
void logout_user(int sockfd, std :: string& received_cookie, std :: string& jwt_token);

#endif // __COMMANDS__HPP__