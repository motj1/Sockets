#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string.h> // Include for strlen and bzero

#define SIZE 1024
#define P_ERROR(message) { perror(message); exit(1); }

int main(int argc, char *argv[]) {
    int status;
    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) P_ERROR("Socket creation error")

    // Attach to port
    int opt = 1;
    status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (status) P_ERROR("setsockopt")

    // bind
    struct sockaddr_in address;
    int port = 8080;

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(port);
    status = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    if (status < 0) P_ERROR("bind failed")

    // listen
    status = listen(server_fd, 3);
    if (status < 0) P_ERROR("listen failed")

    // accept
    int addrlen = sizeof(address);
    int con_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (con_socket < 0) P_ERROR("accept failed")

    // Code
    char buffer[SIZE];

    int n = read(con_socket, buffer, SIZE);
    printf("%s", buffer);

    bzero(buffer, SIZE);

    strncpy(buffer, "rec", 3);

    send(con_socket, buffer, SIZE, 0);

    // Clean up
    close(con_socket); // closing the connected socket
	shutdown(server_fd, SHUT_RDWR); // closing the listening socket

    return 0;
}