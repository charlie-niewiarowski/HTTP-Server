#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>

#include "macros.h"

////////////////////////////////////////////////// UDTS //////////////////////////////////////////////////

/**
 * Http request struct with child headers struct to store HTTP request data
 */

typedef struct {
    char key[MAX_HEADER_KEY_SIZE];
    char value[MAX_HEADER_VAL_SIZE];
} HttpHeader;

typedef struct {
    HttpHeader headers[MAX_HEADERS];
    unsigned int count;
} HttpHeaders;

typedef struct {
    char method[8];
    char path[1024];
    char version[16];

    HttpHeaders headers;
} HttpRequest;


/**
 * ServerSocket struct to transfer both server_fd and server_addr across functions
 */
typedef struct {
    int fd;
    struct sockaddr_in addr;
} ServerSocket;

///////////////////////////////////////////////// METHODS ////////////////////////////////////////////////

/** 
 * @brief Helper function used throughout the server for handling error code responses
 * @param client_fd the client socket to send the response to
 * @param code the response code to send
 * @param message the desired message to indicate what failed
 */
void send_error_response(int client_fd, int code, const char* message);

/**
 * @brief Creates a server socket configured to HTTP standard 
 * Binds to a PORT specified by macros.h
 * Listens for client connections
 * @return ServerSocket struct
 */
ServerSocket serverSocket();

/** 
 * @brief This function handles client requests upon accepting a connection
 * It first receives the bytes from the client
 * Then it parses the request for the HTTP method, path, and version
 * Next it creates a path to a file, opens it, and reads the contents
 * It then constructs and response and sends it to the client
 * @param client_fd the client socket's file number
 * @param client_addr the client socket's address
*/
void handleClient(int client_fd);

/** 
 * @brief Helper function for handleClient
 * Creates an HTTP request object
 * Parses request line
 * Parses following lines as headers
 * @param buffer the bytes to read from
 * @return the HTTP request object
 */
HttpRequest parseRequest(char buffer[BUFFERSIZE]);

/** 
 * @brief Helper function for handleClient
 * Maps a path to a file
 * Reads the data from that file
 * @param req the request containing the path and other data
 * @return the dynamically allocated body that was read from the file
 */
char* readFile(HttpRequest req, size_t* filesize);

const char* get_header_value(HttpHeaders* headers, const char* key);
const char* get_mime_type(const char* path);
void handleResponse(HttpRequest req, char* body, size_t* filesize, int client_fd);