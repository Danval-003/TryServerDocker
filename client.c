#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>

#include <stdint.h>

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

    // Configurar la dirección del servidor
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Cambia la IP por la del servidor si es remoto

    // Conectar al servidor
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error al conectar con el servidor");
        close(clientSocket);
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
    uint8_t type = 1;
    send(clientSocket, (char *)&type, sizeof(uint8_t), 0);
    send(clientSocket, (char *)&sendDataA, sizeof(struct TypeA), 0);

    // Enviar estructura TypeB al servidor
    type = 2;
    send(clientSocket, (char *)&type, sizeof(uint8_t), 0);
    send(clientSocket, (char *)&sendDataB, sizeof(struct TypeB), 0);

    // Enviar estructura TypeC al servidor
    type = 3;
    send(clientSocket, (char *)&type, sizeof(uint8_t), 0);
    send(clientSocket, (char *)&sendDataC, sizeof(struct TypeC), 0);

    // Cerrar el socket
    close(clientSocket);

    return 0;
}
