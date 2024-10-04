## Drinciu Cristina 324CA

# WEB SERVER
This programm develops a WEB server that uses HTTP protocol and that simulates an online library.

This project is developed in C++ for easier use of memory by using std :: string.
Most of this assignment includes code from lab9: HTTP, because the it is based most of it on HTTP requests to the server and HTTP response and parsing the response for checking errors.
Also, for uing the json features, I used the lib #include "nlohmann/json.hpp".

The archive includes the following files:

* buffer.hpp and bufferc.cpp
  * these files implement useful functions for the data structure "buffer"

* helpers.hpp and helpers.cpp
  * these files implement useful functions that work with the server, such as to open a connection, close the connection, to send a message to the server and to receive one from the server
  * includes some macros for Server data, like Ip and port, also the specific paths for each topic

* requests.hpp and requests.cpp
  * these files implement the 3 types of requests used in this project: 
    * GET
    * POST
    * DELETE

* commands.hpp and commands.cpp
  * these files implement the functions that handle each type of command
  * Register: 
    * read the input (username, password) and check if it valid(without whitespaces and not empty); if not, the command fails, returning an error
    * create the json body with the username and password
    * create the body of the message that will be sent to the server, by making the json body compact using dump()(from nlohmann/json.hpp)
    * create the POST request 
    * send the request to the server
    * receive the response from the server
    * extract the response using basic_extract_json_response from buffer.hpp
    * check if there is a response: a valid one is empty, a nonvalid one will be receiving an error

  * Login:
    * the same as Register till receiving te response from the server
    * when extracting the response, the valid one should include the received cookie for login from the server, so store it
    * the extraction is made by finding the "connect.sid" mark and then extract it with strtok by using the delimitator ";"
    * otherwise, it will be received an error
    
  * Enter Library:
    * check if the cookie is present, proof that the user is logged in
    * the same steps until receiving the message, without any input, but create a GET request
    * extract the jwt token, by extracting the json body by using the parse() function and take the field token
    * store the token
    * otherwise, it will be received an error
    
  * Get Books
    * check if the token is present, proof that the user has enetered the library
    * same steps withou the input until receiving the response, but create a GET request
    * extract the json body from the response
    * print the result by using the string format (using dump)
    * otherwise, extract the error from the response
  
  * Get Book
    * check if the token is present, proof that the user has enetered the library
    * read the id
    * build the path, by also cancatenating the id to the books topic path 
    * same steps like Get Books

  * Add Book
    * check if the token is present, proof that the user has enetered the library
    * read the book fields
    * build the json body
    * create a POST request
    * build the server request
    * receive the response
    * the response should be empty if the book was added successfuly
    * otherwise, extract the error
  
  * Delete Book
    * check if the token is present, proof that the user has enetered the library
    * read the book id that is to be deleted
    * build the json body
    * compute a DELETE request
    * send the message to the server
    * receive the response from the server
    * the response should be empty if the book was deleted successfuly
    * otherwise, extract the error

  * Logout
    * check if the cookie is present, proof that the user is logged in
    * compute a GET request
    * send and receive the mssage from the server
    * the response should be empty if the user logged out successfuly
    * otherwise, extract the error
    * mark the token and the cookie empty or NULL
  
* client.cpp
  * here is the flow of the program
  * deckare the cookie and the jwt token, initially empty
  * start a loop
  * open the connection to the server
  * read the command and check is the command is valid
  * if it is valid, call the specific function from commands.cpp to handle the command
  * if it is an "EXIT" command, break the loop
  * close the connection with the server and the socket
    