#include "server.h"

void send_error_response(int client_fd, int code, const char* message) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "HTTP/1.1 %d %s\r\n"
             "Content-Length: 0\r\n"
             "Connection: close\r\n"
             "\r\n",
             code, message);
    send(client_fd, buffer, strlen(buffer), 0);
}

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

HttpRequest parseRequest(char buffer[BUFFERSIZE]) {
    HttpRequest req;
    char* line = strtok(buffer, "\r\n");
    if ((sscanf(buffer, "%s %s %s", req.method, req.path, req.version)) != 3) {
        perror("invalid HTTP request");
    }

    HttpHeaders headers;
    headers.count = 0;

    line = strtok(NULL, "\r\n");
    while (line && strlen(line) > 0) {
        char* colon = strchr(line, ':');
        if (colon) {
            *colon = '\0';
            char* key = line;
            char* value = colon + 1;
            while (*value == ' ') value++;

            strncpy(headers.headers[headers.count].key, key, MAX_HEADER_KEY_SIZE - 1);
            strncpy(headers.headers[headers.count].value, value, MAX_HEADER_VAL_SIZE - 1);
            headers.count++;
        }
        line = strtok(NULL, "\r\n");
    }

    req.headers = headers;
    printf("successfully parsed request\n");
    return req;
}

char* readFile(HttpRequest req, size_t* filesize) {
    char filepath[2048];
    if (strcmp(req.method, "GET") == 0) {
        if (strcmp(req.path, "/") == 0) {
            snprintf(filepath, sizeof(filepath), "webroot/index.html");
        } else {
            snprintf(filepath, sizeof(filepath), "webroot/%s", req.path);
        }
    }
    
    FILE* file = fopen(filepath, "r");
    if (!file) {
        printf("file (%s) open failed: ", filepath);
        perror("");
    }

    fseek(file, 0, SEEK_END);
    *filesize = ftell(file);
    rewind(file);

    char* body = malloc(*filesize);
    size_t read_code = fread(body, 1, *filesize, file);
    if (read_code != *filesize) {
        perror("could not read contents from file\n");
    }

    fclose(file);
    printf("successfully read file\n");
    return body;
}

const char* get_header_value(HttpHeaders* headers, const char* key) {
    for (unsigned int i = 0; i < headers->count; i++) {
        if (strcasecmp(headers->headers[i].key, key) == 0) {
            return headers->headers[i].value;
        }
    }
    return NULL;
}

const char* get_mime_type(const char* path) {
    const char* ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".json") == 0) return "application/json";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".txt") == 0) return "text/plain";


    return "application/octet-stream";
}

void handleResponse(HttpRequest req, char* body, size_t* filesize, int client_fd) {
    char response[BUFFERSIZE];
    int len = 0;

    len += snprintf(response + len, sizeof(response) - len, "HTTP/1.1 200 OK\r\n");

    len += snprintf(response + len, sizeof(response) - len,
                    "Content-Length: %zu\r\n", *filesize);

    const char* mime_type = get_mime_type(req.path);
    printf("%s\n", mime_type);

    if (mime_type) {
        len += snprintf(response + len, sizeof(response) - len,
                        "Content-Type: %s\r\n", mime_type);
    } else {
        len += snprintf(response + len, sizeof(response) - len,
                        "Content-Type: text/html\r\n");
    }

    len += snprintf(response + len, sizeof(response) - len, "\r\n");

    memcpy(response + len, body, *filesize);
    int total_len = len + *filesize;
    
    printf("response: %s\n", response);

    send(client_fd, response, total_len, 0);
    printf("sent response\n");

    free(body);
}

void handleClient(int client_fd) {
    char buffer[BUFFERSIZE] = {0};
    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_received <= 0) {
        goto end_of_function;
    }
    
    buffer[bytes_received] = '\0';
    
    HttpRequest req = parseRequest(buffer);

    size_t filesize = 0;
    char* body = readFile(req, &filesize);

    if (!body) {
        send_error_response(client_fd, 400, "No body in request\n");
    }

    handleResponse(req, body, &filesize, client_fd);

end_of_function:
    printf("closing client connection\n");
    close(client_fd);
}

