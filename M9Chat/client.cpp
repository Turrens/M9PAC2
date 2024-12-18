#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
 
#pragma comment(lib, "Ws2_32.lib") // Enllacem la llibreria de Winsock
 
#define PORT 6565
 
using namespace std;
 
void receive_messages(SOCKET client_socket) {
    char buffer[1024];
    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            cout << "Missatge del servidor: " << buffer << endl;
        } else {
            cerr << "S'ha perdut la connexió amb el servidor.\n";
            break;
        }
    }
}
 
int main() {
    WSADATA wsaData;
    SOCKET client_socket;
    struct sockaddr_in server_address;
 
    // Inicialitzar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error en la inicialització de Winsock.\n";
        return -1;
    }
 
    // Crear el socket del client
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        cerr << "Error al crear el socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }
 
    // Configurar la informació del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("172.17.20.181"); // Adreça IP de loopback (local)
 
    // Connectar-se al servidor
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        cerr << "Error al connectar amb el servidor: " << WSAGetLastError() << "\n";
        closesocket(client_socket);
        WSACleanup();
        return -1;
    }
 
    cout << "Connexió establerta amb el servidor.\n";
 
    // Crear un fil per rebre missatges
    thread receiver(receive_messages, client_socket);
    receiver.detach();
 
    string message;
    while (true) {
        cout << "Escriu un missatge (o '/exit' per sortir): ";
        getline(cin, message);
        if (message == "/exit") {
            break;
        }
        send(client_socket, message.c_str(), message.length(), 0);
    }
 
    closesocket(client_socket);
    WSACleanup();
    return 0;
}