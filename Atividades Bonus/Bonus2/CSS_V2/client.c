#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

typedef struct {
    double op1;
    double op2;
    char operation;
} CalcArgs;

void* handle_request(void* args_void) {
    CalcArgs* args = (CalcArgs*)args_void;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        free(args);
        pthread_exit(NULL);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "192.168.100.36", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        free(args);
        pthread_exit(NULL);
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        free(args);
        pthread_exit(NULL);
    }

    sprintf(buffer, "%lf %c %lf", args->op1, args->operation, args->op2);
    send(sock, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    read(sock, buffer, 1024);
    printf("Result for %.2lf %c %.2lf = %s\n", args->op1, args->operation, args->op2, buffer);

    close(sock);
    free(args);
    pthread_exit(NULL);
}

int main() {
    while (1) {
        double op1, op2;
        char operation;

        printf("\nEnter: num1 op num2 (e.g. 5 + 5): ");
        if (scanf("%lf %c %lf", &op1, &operation, &op2) != 3) {
            printf("Invalid input. Try again.\n");
            while (getchar() != '\n'); // limpa buffer
            continue;
        }

        CalcArgs* args = malloc(sizeof(CalcArgs));
        args->op1 = op1;
        args->op2 = op2;
        args->operation = operation;

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_request, args) != 0) {
            perror("Failed to create thread");
            free(args);
        } else {
            pthread_detach(tid); // auto-limpeza
        }
    }

    return 0;
}
