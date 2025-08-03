#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <netinet/in.h>
#include <unistd.h>
#define PORT 8080

double calculate(char operation, double op1, double op2);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            continue;
        }

        printf("Client connected.\n");

        char buffer[1024] = {0};
        double op1, op2;
        char operation;

        int bytes_read;
        while ((bytes_read = read(new_socket, buffer, 1024)) > 0) {
            sscanf(buffer, "%lf %c %lf", &op1, &operation, &op2);
            double result = calculate(operation, op1, op2);
            sprintf(buffer, "%lf", result);
            send(new_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
        }

        printf("Client disconnected.\n");
        close(new_socket);
    }

    return 0;
}

double calculate(char operation, double op1, double op2) {
    double result = 0;
    switch(operation) {
        case '+': result = op1 + op2; break;
        case '-': result = op1 - op2; break;
        case '*': result = op1 * op2; break;
        case '/': result = (op2 != 0) ? op1 / op2 : 0; break;
        default: printf("Invalid operation\n");
    }
    return result;
}
