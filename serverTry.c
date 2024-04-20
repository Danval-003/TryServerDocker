#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h> // Librería para manejar hilos
#include <signal.h>  // Librería para manejar señales

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


// Función para manejar la conexión con un cliente en un hilo separado
void *handleClient(void *arg) {
    int clientSocket = *(int *)arg;
    free(arg);

    // Recibir estructuras del cliente
    while (1) {
        uint8_t messageType;
        int bytesReceived = recv(clientSocket, &messageType, sizeof(uint8_t), 0);
        if (bytesReceived <= 0) {
            printf("Cliente desconectado\n");
            break;
        }

        // Determinar qué tipo de estructura se recibió
        switch (messageType) {
            case TYPE_A: {
                struct TypeA data;
                bytesReceived = recv(clientSocket, (char *)&data, sizeof(struct TypeA), 0);
                if (bytesReceived <= 0) {
                    printf("Error al recibir datos de TypeA\n");
                    break;
                }
                int dataInt;
                // Copiar los bytes de intBytes de vuelta a restoredInteger
                memcpy(&dataInt, data.data, sizeof(int));
                printf("Datos recibidos de TypeA: data1 = %d\n", dataInt);
                break;
            }
            case TYPE_B: {
                struct TypeB data;
                bytesReceived = recv(clientSocket, (char *)&data, sizeof(struct TypeB), 0);
                if (bytesReceived <= 0) {
                    printf("Error al recibir datos de TypeB\n");
                    break;
                }
                float data1;
                float data2;

                memcpy(&data1, data.data1, sizeof(float));
                memcpy(&data2, data.data2, sizeof(float));

                printf("Datos recibidos de TypeB: data1 = %f, data2 = %f\n", data1, data2);
                break;
            }
            case TYPE_C: {
                struct TypeC data;
                bytesReceived = recv(clientSocket, (char *)&data, sizeof(struct TypeC), 0);
                if (bytesReceived <= 0) {
                    printf("Error al recibir datos de TypeC\n");
                    break;
                }
                printf("Datos recibidos de TypeC: data = %s\n", data.data);
                break;
            }
            default:
                printf("Tipo de estructura desconocido\n");
                break;
        }
    }

    // Cerrar el socket del cliente
    close(clientSocket);
    pthread_exit(NULL);
}

// Manejador de señales para detener el servidor
void sigintHandler(int sig) {
    printf("\nDeteniendo el servidor...\n");
    exit(0);
}

int main() {
    // Registrar el manejador de señales para Ctrl+C
    signal(SIGINT, sigintHandler);

    // Crear un socket TCP
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error al crear el socket");
        return 1;
    }

    // Configurar la dirección del servidor
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8000);

    // Vincular el socket a la dirección y puerto
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error al vincular el socket");
        close(serverSocket);
        return 1;
    }

    // Escuchar conexiones entrantes
    if (listen(serverSocket, 5) == -1) {
        perror("Error al escuchar en el socket");
        close(serverSocket);
        return 1;
    }

    printf("Servidor esperando conexiones...\n");

    // Aceptar conexiones entrantes en un bucle infinito
    while (1) {
        // Aceptar la conexión entrante
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int *clientSocket = (int *)malloc(sizeof(int)); // Crear memoria dinámica para el socket del cliente
        *clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (*clientSocket == -1) {
            perror("Error al aceptar la conexión");
            close(serverSocket);
            free(clientSocket);
            return 1;
        }

        // Crear un hilo para manejar la conexión con el cliente
        pthread_t tid;
        if (pthread_create(&tid, NULL, handleClient, (void *)clientSocket) != 0) {
            perror("Error al crear el hilo");
            close(*clientSocket);
            free(clientSocket);
        }
    }

    // Cerrar el socket del servidor (este código nunca se ejecuta debido al bucle infinito)
    close(serverSocket);

    return 0;
}
