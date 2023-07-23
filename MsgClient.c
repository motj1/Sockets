#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/random.h>

#define SIZE 1024
#define P_ERROR(message) { perror(message); exit(1); }

int client_fd;

void Gencrypt(char plaintext[], int key) {
    for (int i = 0; plaintext[i] != '\0'; i ++) { plaintext[i] += key; }
}

// Function to decrypt the ciphertext using a Caesar cipher
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

void start(const char *ip) {
    // Create a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) P_ERROR("Socket creation error")

    // Convert IPv4 and IPv6 addresses from text to binary form
    // const char *ip = "192.168.0.115";
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
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Please add the ip of the server and your login name and password: ./Msg ip name password");
        return 1;
    }

    start(argv[1]);

    char buffer[SIZE] = "Hello server from client!";

    int n = send(client_fd, buffer, SIZE, 0);

    int valread = read(client_fd, buffer, SIZE);

    bzero(buffer, SIZE);

    n = send(client_fd, argv[2], strlen(argv[2]), 0);

    valread = read(client_fd, buffer, SIZE);

    bzero(buffer, SIZE);

    n = send(client_fd, argv[3], strlen(argv[3]), 0);

    valread = read(client_fd, buffer, SIZE);

    if (!strcmp("no", buffer)) { close(client_fd); return 0; }
    else { bzero(buffer, SIZE); }

    char public[SIZE];

    valread = read(client_fd, buffer, SIZE);
    
    int o_pkey = atoi(buffer);

    bzero(buffer, SIZE);
    
    int pkey = getRandomNumber(19, 119);

    sprintf(buffer, "%d", pkey);

    n = send(client_fd, buffer, SIZE, 0);
    bzero(buffer, SIZE);

    n = read(client_fd, buffer, SIZE);

    system("clear");

    FILE* file = fopen("welcome.txt", "r");
    char line[256];
    while (fgets(line, sizeof(line), file)) printf("%s", line); 
    fclose(file);

    printf("Logged in, Hello %s\n\n", argv[2]);
    printf("Type your first message: ");

    while (1) {
        char *line = NULL;
        size_t len = 0;
        ssize_t lineSize = 0;
        printf("\n > ");
        lineSize = getline(&line, &len, stdin);

        if (lineSize <= 1) break;
        if (!strcmp("q\n", line)) break;

        Gencrypt(line, o_pkey);

        n = send(client_fd, line, lineSize, 0);

        valread = read(client_fd, buffer, SIZE);

        if (!strcmp("EOF", buffer) || !strcmp("EOF\n", buffer)) break;

        Gdecrypt(buffer, pkey);

        printf("Server: %s", buffer);

        bzero(buffer, SIZE);

        free(line);
    }

    char End[4] = "EOF";
    Gencrypt(End, o_pkey);
    n = send(client_fd, End, strlen(End), 0);

    /*
    To read from file use:

    fread(data, SIZE, 1, fp);

    Otherwise it will only read from one line
    */

    // Clean up

    close(client_fd); // closing the connected socket

    return 0;
}