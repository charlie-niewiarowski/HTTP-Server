#include <signal.h>

#include "server.h"

int main() {
    ServerSocket server = serverSocket();
    signal(SIGCHLD, SIG_IGN);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server.fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd == -1) {
            perror("could not connect to client socket\n");
        } else {
            printf("connected to client with fd %d\n", client_fd);
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("could not fork processes");
        } else if (pid == 0) {
            close(server.fd);
            handleClient(client_fd);
            exit(0);
        } else {
            close(client_fd);
        }
    }

    return 0;
}