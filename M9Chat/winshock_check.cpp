#include <winsock2.h>
#include <iostream>
#include <thread>
#include <cstring>

#pragma comment(lib, "Ws2_32.lib")

void receive_messages(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Desconnectat del servidor.\n";
            return;
        }

        buffer[bytesReceived] = '\0';
        std::cout << "Missatge del servidor: " << buffer << "\n";
    }
}

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char message[1024];

    // Inicialitzar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error al inicialitzar Winsock.\n";
        return -1;
    }

    // Crear el socket del client
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }

    // Configurar l'adreça del servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connectar al servidor
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error al connectar amb el servidor: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Connectat al servidor de xat.\n";

    // Crear un fil per rebre missatges
    std::thread(receive_messages, clientSocket).detach();

    // Enviar missatges al servidor
    while (true) {
        std::cin.getline(message, sizeof(message));
        send(clientSocket, message, strlen(message), 0);
    }

    // Tancar el socket del client
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
