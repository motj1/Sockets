#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string.h> // Include for strlen and bzero

#define SIZE 1024
#define P_ERROR(message) { perror(message); exit(1); }

struct sockaddr_in address;

int start() {
    int status;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) P_ERROR("Socket creation error")

    // Attach to port
    int opt = 1;
    status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (status) P_ERROR("setsockopt")

    // bind
    int port = 8080;

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(port);
    status = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    if (status < 0) P_ERROR("bind failed")

    // listen
    status = listen(server_fd, 3);
    if (status < 0) P_ERROR("listen failed")

    return server_fd;
}

int accept_connection(int server_fd) {
    // accept
    int addrlen = sizeof(address);
    int con_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (con_socket < 0) P_ERROR("accept failed")
    
    return con_socket;
}


void send_file(FILE *fp, int sockfd) {
    int n, i = 0;
    char data[SIZE] = { 0 };
    fread(data, SIZE, 1, fp);
    do {
        n = send(sockfd, data, sizeof(data), 0);
        if (n == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        // if (i == 0) { printf("%s", data); }
        i += n;
        printf("\rBytes processed: %d", i);
        fflush(stdout);

        bzero(data, SIZE);
    } while(fread(data, SIZE, 1, fp));

    printf("\n");

    return;
}

void handle_connection(int con_socket) {
    // Code
    char buffer[SIZE];

    int n = read(con_socket, buffer, SIZE);
    printf("%s\n", buffer);

    struct stat tmpbuffer;
    if (stat(buffer, &tmpbuffer) == 0) {
        FILE * fp = fopen(buffer, "r");
        send_file(fp, con_socket);
        fclose(fp);
    } else {
        FILE * fp = fopen("Howto.txt", "r");
        send_file(fp, con_socket);
        fclose(fp);
    }

    bzero(buffer, SIZE);

    strncpy(buffer, "EOF", 3);

    send(con_socket, buffer, SIZE, 0);
    // Clean up
    close(con_socket); // closing the connected socket
}

int main(int argc, char *argv[]) {
    // Create socket
    int server_fd = start();

    while (1) {
        printf("Connecting to new client\n");
        int con_socket = accept_connection(server_fd);
        handle_connection(con_socket);
    }

	shutdown(server_fd, SHUT_RDWR); // closing the listening socket

    return 0;
}