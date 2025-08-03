#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char operation;
    double op1, op2;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "192.168.100.36", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1){
        printf("\n\nEnter: num1 op num2 (e.g. 5 + 5): ");
        scanf("%lf %c %lf", &op1, &operation, &op2);
        sprintf(buffer, "%lf %c %lf", op1, operation, op2);
        send(sock, buffer, strlen(buffer), 0);
        printf("Request sent\n");
        memset(buffer, 0, sizeof(buffer));
        read(sock, buffer, 1024);
        printf("Result: %s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
    }

    return 0;
}
