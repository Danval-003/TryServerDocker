#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <netdb.h> // Para getaddrinfo()

// Tipo de estructuras
#define TYPE_A 1
#define TYPE_B 2
#define TYPE_C 3
#define TYPE_D 4
#define TYPE_E 5
#define TYPE_F 6

// Estructura base con atributo type
struct Base {
    uint8_t type; // Valor entre 1 y 6
};

// Estructuras específicas derivadas de Base
struct TypeA {
    char data[sizeof(int)];
};

struct TypeB {
    char data1[sizeof(float)];
    char data2[sizeof(float)];
};

struct TypeC {
    char data[256];
};

// Puedes definir más estructuras TypeD, TypeE, TypeF según sea necesario


int main() {
    // Crear un socket TCP
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error al crear el socket");
        return 1;
    }

    // Resolución de la URL del servidor a una dirección IP
    struct addrinfo hints;
    struct addrinfo *serverInfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo("unaware-swordfish-dsval.koyeb.app", "443", &hints, &serverInfo);
    if (status != 0) {
        fprintf(stderr, "Error al resolver la dirección: %s\n", gai_strerror(status));
        close(clientSocket);
        return 1;
    }

    // Configurar la dirección del servidor
    struct sockaddr_in *serverAddr = (struct sockaddr_in *)serverInfo->ai_addr;

    // Conectar al servidor
    if (connect(clientSocket, (struct sockaddr *)serverAddr, sizeof(struct sockaddr_in)) == -1) {
        perror("Error al conectar con el servidor");
        close(clientSocket);
        freeaddrinfo(serverInfo);
        return 1;
    }

    printf("Conectado al servidor\n");

    float number = 3.14159f;
    float number2 = 2.71;
    int number3 = 23;

    // Ejemplo de envío de estructuras
    struct TypeA sendDataA;
    memcpy(sendDataA.data, &number3, sizeof(int));
    struct TypeB sendDataB;
    memcpy(sendDataB.data1, &number, sizeof(float));
    memcpy(sendDataB.data2, &number2, sizeof(float));
    struct TypeC sendDataC = { "Hello"};

    // Enviar estructura TypeA al servidor
    uint8_t type = TYPE_A;
    send(clientSocket, (char *)&type, sizeof(uint8_t), 0);
    send(clientSocket, (char *)&sendDataA, sizeof(struct TypeA), 0);
    // Recibir respuesta del servidor
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1) {
        perror("Error al recibir respuesta del servidor");
    } else if (bytesReceived == 0) {
        printf("Servidor desconectado\n");
    } else {
        buffer[bytesReceived] = '\0';
        printf("Respuesta del servidor: %s\n", buffer);
    }

    // Enviar estructura TypeB al servidor
    type = TYPE_B;
    send(clientSocket, (char *)&type, sizeof(uint8_t), 0);
    send(clientSocket, (char *)&sendDataB, sizeof(struct TypeB), 0);
    // Recibir respuesta del servidor
    bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1) {
        perror("Error al recibir respuesta del servidor");
    } else if (bytesReceived == 0) {
        printf("Servidor desconectado\n");
    } else {
        buffer[bytesReceived] = '\0';
        printf("Respuesta del servidor: %s\n", buffer);
    }

    // Enviar estructura TypeC al servidor
    type = TYPE_C;
    send(clientSocket, (char *)&type, sizeof(uint8_t), 0);
    send(clientSocket, (char *)&sendDataC, sizeof(struct TypeC), 0);

    bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1) {
        perror("Error al recibir respuesta del servidor");
    } else if (bytesReceived == 0) {
        printf("Servidor desconectado\n");
    } else {
        buffer[bytesReceived] = '\0';
        printf("Respuesta del servidor: %s\n", buffer);
    }

    // Cerrar el socket y liberar la memoria de la información del servidor
    close(clientSocket);
    freeaddrinfo(serverInfo);

    return 0;
}
