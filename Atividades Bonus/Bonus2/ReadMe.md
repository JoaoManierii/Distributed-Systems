# Bônus #2

## IPC Remoto (Comunicação entre Processos Remotos)

Como explicado previamente, o entendimento de como se dá a programação de IPC (Interprocess communication) remoto é muito importante no contexto de sistemas distribuídos.

A base para esse entendimento advém das disciplinas de SO (IPC) e Redes de computadores (TCP/IP).

A fim de revisar/consolidar esses conceitos, abaixo apresento uma atividade a ser desenvolvida extraclasse, na categoria de bônus.

Por ser um bônus, trata-se de uma atividade facultativa. Apesar de ser uma atividade facultativa, realizar esta atividade ajudará sobremaneira a entender e fixar diversos conceitos que estudamos na disciplina até então.

O objetivo dessa atividade é a programação de uma aplicação cliente-servidor, a qual será composta de dois programas em Linguagem C (`cliente.c` e `servidor.c`)

O programa cliente (`cliente.c`) serve para requisitar serviços implementados pelo programa servidor (`servidor.c`), o qual executa tais serviços e envia a resposta para o cliente que é o responsável por apresentar o resultado ao usuário.

Como o foco aqui é na compreensão dos mecanismos de comunicação, a implementação dos serviços, bem como aspectos de interface com o usuário no lado cliente, são questões secundárias.

Para tanto, adotaremos o exemplo de uma calculadora. Ou seja, a aplicação cliente apresenta as opções de operações de uma calculadora para o usuário que seleciona a operação desejada e entra com os parâmetros da operação. Ao clicar "=", o programa cliente envia para o programa servidor a operação a ser executada, juntamente com os seus respectivos operandos, ficando na sequência aguardando pelo resultado. O servidor, por sua vez, se comporta da seguinte forma: aguarda a chegada de uma requisição, identifica a operação e a executa com os valores dos operandos recebidos do cliente. Após concluir a operação solicitada, o servidor envia a resposta para o cliente.

Neste exercício, as operações suportadas pela calculadora são: adição, multiplicação, radiciação, potenciação e fatorial.

### Note que serão implementadas diferentes versões da aplicação Calculadora Cliente-Servidor (CC-S):

#### CCS v1 (Cliente e Servidor Singlethreaded, processamento iterativo nos dois lados):
- **Cliente**: Looping (Lê os dados, executa a requisição, aguarda a resposta, imprime a resposta na tela)
- **Servidor**: Looping (Aguarda requisição do cliente, executa a requisição, envia a resposta para o cliente)

#### CCS v2 (Cliente Multithreaded e Servidor Singlethreaded, processamento simultâneo no cliente e iterativo no servidor):
- **Cliente**: Cria uma thread para cada requisição realizada, portanto, não bloqueando o cliente depois que envia a requisição para o servidor. Desta forma, o usuário pode iniciar várias operações simultâneas no lado do cliente.
- **Servidor**: Looping (Aguarda requisição do cliente, executa a requisição, envia a resposta para o cliente).

#### CCS v3 (Cliente Multithreaded e Servidor Multithreaded, processamento simultâneo nos dois lados):
- **Cliente**: Cria uma thread para cada requisição realizada, portanto, não bloqueando o cliente depois que envia a requisição para o servidor. Desta forma, o usuário pode iniciar várias operações simultâneas no lado do cliente.
- **Servidor**: Cria uma thread para cada requisição que chega, executando todas as requisições de forma simultânea.

#### CCS v4 (Cliente Multithreaded e Servidor Singlethreaded do tipo FSM, processamento simultâneo nos dois lados):
- **Cliente**: Cria uma thread para cada requisição realizada, portanto, não bloqueando o cliente depois que envia a requisição para o servidor. Desta forma, o usuário pode iniciar várias operações simultâneas no lado do cliente.
- **Servidor**: Apesar de usar uma única thread para todas as requisições que chegam, executa todas as requisições de forma simultânea seguindo uma abordagem de Finite-State Machine.

---

## Ambiente de execução:

O programa cliente deve executar em uma máquina (física ou virtual) diferente da máquina (física ou virtual) do programa servidor.
Por conseguinte, ambos estarão utilizando endereços de rede (IP) diferentes.

O código apresentado abaixo foi escrito para executar no sistema operacional Linux (qualquer distribuição).

---

## Código base:

Para realizar a comunicação entre processos (IPC) de forma remota, utilizaremos a interface sockets para acessar serviços da pilha de protocolos TCP/IP. A interface sockets é atualmente implementada por todos os sistemas operacionais modernos.

### Exemplo de Código do Programa Servidor (v1):

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <netinet/in.h>
#define PORT 8080

double calculate(char operation, double operand);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    char buffer[1024] = {0};
    double operand;
    char operation;

    while(1) {
        read(new_socket, buffer, 1024);
        sscanf(buffer, "%c %lf", &operation, &operand);
        double result = calculate(operation, operand);
        sprintf(buffer, "%lf", result);
        send(new_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    return 0;
}

double calculate(char operation, double operand) {
    double result = 0;
    switch(operation) {
        case '+':
            result = operand + operand;
            break;
        case '*':
            result = operand * operand;
            break;
        case 's':
            result = sqrt(operand);
            break;
        case '^':
            result = exp(operand);
            break;
        case '!':
            result = 1;
            for(int i = 1; i <= operand; i++) {
                result *= i;
            }
            break;
        default:
            printf("Invalid operation\n");
    }
    return result;
}
```

### Exemplo de Código do Programa Cliente (v1):

```c
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
```

**OBS**: Onde aparece `127.0.0.1` no código do cliente, substitua pelo endereço IP do host onde o servidor estará executando no seu ambiente de execução.

---

## IMPORTANTE:

O código-fonte que aparece neste post foi gerado originalmente com auxílio do ChatGPT (chat.openai.com), contendo modificações realizadas pelo professor. Fique livre para usar este código como base para implementar as demais versões do programa (CCS v2/v3/v4).

---

## Entregas:

- Relatório em PDF descrevendo o código produzido, para cada versão. Não é código comentado, mas sim a descrição do código de forma textual.
- Código-fonte (arquivo `.zip`)
- Apresentação para o professor c/ arguição.

### Datas:
- Relatório: **03/08**
- Apresentação: **08/08**, 18h, Sala 1B-129.

Na apresentação o aluno deve estar com um computador apto a executar e compilar o código apresentado.

**IMPORTANTE**: Ler seção **INTEGRIDADE ACADÊMICA** do Plano de Ensino. O professor reserva-se ao direito de atribuir nota zero a todos os trabalhos que apresentarem similaridade entre si que sugere cópia.
