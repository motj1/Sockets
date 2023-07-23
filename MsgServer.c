#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/random.h>

#define SIZE 1024
#define P_ERROR(message) { perror(message); exit(1); }

int server_fd;
struct sockaddr_in address;

void Gencrypt(char plaintext[], int key) {
    for (int i = 0; plaintext[i] != '\0'; i ++) { plaintext[i] += key; }
}

void Gdecrypt(char ciphertext[], int key) {
    for (int i = 0; ciphertext[i] != '\0'; i ++) { ciphertext[i] -= key; }
}

int getRandomNumber(int min, int max) {
    unsigned char buffer[4]; // Using 4 bytes to get 32-bit random data
    int result = getentropy(buffer, sizeof(buffer));
    unsigned int randomValue = *(unsigned int*)buffer;
    int randomNumber = min + randomValue % (max - min + 1);
    return randomNumber;
}

void start() {
    int status;
    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
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

    // accept
    
}

int main(int argc, char *argv[]) {
    start();

    while (1) {
    printf("Connect to client? \n");
    char in = getc(stdin);
    fflush(stdin);
    if (in == 'q' ) { break; }
    int addrlen = sizeof(address);
    int con_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (con_socket < 0) P_ERROR("accept failed")

    char buffer[SIZE];

    int exitNow = 0;

    for (int i=0; i<3; i++) {
        bzero(buffer, SIZE);
        int n = read(con_socket, buffer, SIZE);
        printf("%s\n", buffer);

        bzero(buffer, SIZE);


        if (i == 2) {
            printf("Accept? ");
            char in = getc(stdin);
            if (in != 'n' && in != 'N')
                strncpy(buffer, "rec", 3);
            else {
                strncpy(buffer, "no", 3);
                exitNow = 1;
            }
        } else {
            strncpy(buffer, "rec", 3);
        }

        send(con_socket, buffer, SIZE, 0);
    }

    bzero(buffer, SIZE);

    int pkey = getRandomNumber(13, 113);

    sprintf(buffer, "%d", pkey);

    send(con_socket, buffer, SIZE, 0);
    bzero(buffer, SIZE);

    int n = read(con_socket, buffer, SIZE);

    int o_pkey = atoi(buffer);

    bzero(buffer, SIZE);

    strncpy(buffer, "rec", 3);

    send(con_socket, buffer, SIZE, 0);

    system("clear");
    FILE* file = fopen("welcome.txt", "r");
    char line[256];
    while (fgets(line, sizeof(line), file)) printf("%s", line); 
    fclose(file);

    while (exitNow == 0) {
        n = read(con_socket, buffer, SIZE);
        Gdecrypt(buffer, pkey);
        if (!strcmp("EOF", buffer) || !strcmp("EOF\n", buffer)) break;

        printf("Client: %s", buffer);

        bzero(buffer, SIZE);

        char *line = NULL;
        size_t len = 0;
        ssize_t lineSize = 0;
        printf("\n > ");
        lineSize = getline(&line, &len, stdin);

        if (!strcmp("q\n", line)) break;

        strncpy(buffer, line, lineSize);
        free(line);

        Gencrypt(buffer, o_pkey);

        send(con_socket, buffer, SIZE, 0);
        bzero(buffer, SIZE);
    }

    // Clean up
    close(con_socket); // closing the connected socket
    }
	shutdown(server_fd, SHUT_RDWR); // closing the listening socket

    return 0;
}
