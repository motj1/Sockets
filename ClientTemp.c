#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <string.h> // Include for bzero

#define SIZE 1024
#define P_ERROR(message) { perror(message); exit(1); }

int main(int argc, char *argv[]) {
    // Create a socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) P_ERROR("Socket creation error")

    // Convert IPv4 and IPv6 addresses from text to binary form
    const char *ip = "192.168.0.115";
    int port = 8080;
    struct sockaddr_in serv_addr;
    int status;

    serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

    status = inet_pton(AF_INET, ip, &serv_addr.sin_addr);
    if (status < 0) P_ERROR("Invalid address/ Address not supported")

    // Connect to server
    status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (status < 0) P_ERROR("Connection Failed")

    // Code

    char buffer[SIZE] = "Hello server!";

    int n = send(client_fd, buffer, SIZE, 0);

    bzero(buffer, SIZE);

    int valread = read(client_fd, buffer, SIZE);

    printf("%s", buffer);

    /*
    To read from file use:

    fread(data, SIZE, 1, fp);

    Otherwise it will only read from one line
    */

    // Clean up

    close(client_fd); // closing the connected socket

    return 0;
}