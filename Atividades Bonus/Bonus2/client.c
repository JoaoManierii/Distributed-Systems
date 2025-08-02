#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char operation;
    double operand;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1) {
        printf("\n\nEnter operation and operand: ");
        scanf("%c %lf", &operation, &operand);
        sprintf(buffer, "%c %lf", operation, operand);
        send(sock, buffer, strlen(buffer), 0);
        printf("Request sent\n");
        memset(buffer, 0, sizeof(buffer));
        read(sock, buffer, 1024);
        printf("Result: %s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
    }

    return 0;
}