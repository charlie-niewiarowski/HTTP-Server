#include "server.h"

ServerSocket serverSocket() {
    int server_fd;
    struct sockaddr_in server_addr;

    // creating socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    } else {
        printf("socket created\n");
    }

    // config socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // bind socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    } else {
        printf("socket binded to port: %d\n", PORT);
    }

    // listen for client connections
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("could not listen for connections");
        exit(EXIT_FAILURE);
    } else {
        printf("socket listenting for connections\n");
    }

    ServerSocket ss;
    ss.addr = server_addr; ss.fd = server_fd;
    return ss;
}

void handle_client(int client_fd) {
    char buffer[BUFFERSIZE] = {0};
    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        
        HttpRequest req;
        if ((sscanf(buffer, "%s %s %s", req.method, req.path, req.version)) != 3) {
            perror("invalid HTTP request");
        }

        char filepath[2048];
        if (strcmp(req.method, "GET") == 0) {
            if (strcmp(req.path, "/") == 0) {
                snprintf(filepath, sizeof(filepath), "static/index.html");
            } else {
                snprintf(filepath, sizeof(filepath), "static/%s", req.path);
            }
        }

        FILE* file = fopen(filepath, "r");
        if (!file) {
            printf("file (%s) open failed: ", filepath);
            perror("");
            return;
        }

        fseek(file, 0, SEEK_END);
        long filesize = ftell(file);
        rewind(file);

        char* body = malloc(filesize);
        long read_code = fread(body, 1, filesize, file);
        if (read_code != filesize) {
            perror("could not read contents from file");
            free(body);
            close(client_fd);
            return;
        }

        fclose(file);

        char response[BUFFERSIZE];
        int len = snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: %ld\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n", filesize);

        memcpy(response + len, body, filesize);
        int total_len = len + filesize;

        send(client_fd, response, total_len, 0);

        free(body);
    }

    close(client_fd);
}