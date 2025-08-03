#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080

typedef struct {
    int client_socket;
    int thread_id;
} ThreadArgs;

int thread_counter = 1;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

double calculate(char operation, double op1, double op2) {
    double result = 0;
    switch(operation) {
        case '+': result = op1 + op2; break;
        case '-': result = op1 - op2; break;
        case '*': result = op1 * op2; break;
        case '/': result = (op2 != 0) ? op1 / op2 : 0; break;
        default: printf("[Thread ?] Invalid operation\n");
    }
    return result;
}

void* handle_client(void* args_void) {
    ThreadArgs* args = (ThreadArgs*)args_void;
    int client_socket = args->client_socket;
    int tid = args->thread_id;
    free(args);

    char buffer[1024] = {0};
    double op1, op2;
    char operation;

    printf("[Thread %d] Client connected.\n", tid);

    int bytes_read;
    while ((bytes_read = read(client_socket, buffer, 1024)) > 0) {
        sscanf(buffer, "%lf %c %lf", &op1, &operation, &op2);
        double result = calculate(operation, op1, op2);
        sprintf(buffer, "%lf", result);
        send(client_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    printf("[Thread %d] Client disconnected.\n", tid);
    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_fd;
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

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            continue;
        }

        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->client_socket = new_socket;

        pthread_mutex_lock(&counter_mutex);
        args->thread_id = thread_counter++;
        pthread_mutex_unlock(&counter_mutex);

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, args) != 0) {
            perror("pthread_create failed");
            close(new_socket);
            free(args);
        } else {
            pthread_detach(tid); // deixa a thread se limpar sozinha
        }
    }

    return 0;
}
