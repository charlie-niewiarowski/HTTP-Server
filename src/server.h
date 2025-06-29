#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "macros.h"

/**
 * Http request struct to store HTTP request data
 */
typedef struct {
    char method[8];
    char path[1024];
    char version[16];
} HttpRequest;


/**
 * ServerSocket struct to transfer both server_fd and server_addr across functions
 */
typedef struct {
    int fd;
    struct sockaddr_in addr;
} ServerSocket;

/**
 * Creates a server socket configured to HTTP standard 
 * Binds to a PORT specified by macros.h
 * Listens for client connections
 * @return ServerSocket struct
 */
ServerSocket serverSocket();

/** Handling client 
 * @brief This function handles client requests upon accepting a connection
 * It first receives the bytes from the client
 * Then it parses the request for the HTTP method, path, and version
 * Next it creates a path to a file, opens it, and reads the contents
 * It then constructs and response and sends it to the client
 * @param client_fd the client socket's file number
 * @param client_addr the client socket's address
 * @return void
*/
void handle_client(int client_fd);