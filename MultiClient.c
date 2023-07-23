#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <string.h> // Include for bzero

#define SIZE 1024
#define P_ERROR(message) { perror(message); exit(1); }

int connect_to_ser() {
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

    return client_fd;
}

void writeFile(int sockfd, char * filename) {
    int n, i;
    char buffer[SIZE];
    FILE *fp = fopen(filename, "w");

    while (1) {
        n = read(sockfd, buffer, SIZE);
        if (n <= 0)
            break;
        fprintf(fp, "%s", buffer);

        i += n;
        printf("Bytes processed: %d", i);
        fflush(stdout);

        bzero(buffer, SIZE);
    }
    fclose(fp);

    printf("\n");

    return;
}

void handle_conn(int client_fd, char * filename) {
    char buffer[SIZE] = { 0 };

    int n = send(client_fd, filename, strlen(filename), 0);

    writeFile(client_fd, filename);

    int valread = read(client_fd, buffer, SIZE);

    printf("%s\n", buffer);
}

int main(int argc, char *argv[]) {
    if (argc < 2) { printf("Please run client with filename\n"); return 1; }

    int client_fd = connect_to_ser();
    handle_conn(client_fd, argv[1]);
    close(client_fd); // closing the connected socket

    // fread(data, SIZE, 1, fp);

    return 0;
}