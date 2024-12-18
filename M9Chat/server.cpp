#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
 
#pragma comment(lib, "Ws2_32.lib") // Enllacem la llibreria de Winsock
 
#define PORT 6565
#define MAX_CLIENTS 5
 
using namespace std;
 
void handle_client(SOCKET client_socket) {
    char buffer[1024];
    int bytes_received;
    while (true) {
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == SOCKET_ERROR) {
            cerr << "Error al rebre el missatge del client: " << WSAGetLastError() << endl;
            break;
        }
        if (bytes_received == 0) {
            cout << "El client s'ha desconnectat." << endl;
            break;
        }
        buffer[bytes_received] = '\0';  // Assegura que la cadena estigui terminada amb '\0'
        cout << "Missatge rebut del client: " << buffer << endl;
        // Envia un missatge de tornada al client
        send(client_socket, buffer, bytes_received, 0);
    }
    closesocket(client_socket);
}
 
int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    sockaddr_in server_address, client_address;
    int client_address_size = sizeof(client_address);
 
    // Inicialitzar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error en la inicialització de Winsock." << endl;
        return -1;
    }
 
    // Crear el socket del servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        cerr << "Error al crear el socket del servidor: " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }
 
    // Configurar la informació del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY; // Accepta connexions des de qualsevol IP
 
    // Vincula el socket a l'adreça i port
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        cerr << "Error al vincular el socket: " << WSAGetLastError() << endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }
 
    // Posar el servidor en mode "listen"
    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR) {
        cerr << "Error en l'escucha de connexions: " << WSAGetLastError() << endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }
 
    cout << "Servidor en marxa, esperant connexions a través del port " << PORT << "..." << endl;
 
    // Acceptar connexions dels clients
    while (true) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_size);
        if (client_socket == INVALID_SOCKET) {
            cerr << "Error en acceptar la connexió del client: " << WSAGetLastError() << endl;
            continue;
        }
        cout << "Nou client connectat!" << endl;
 
        // Crear un nou fil per manejar el client
        thread client_thread(handle_client, client_socket);
        client_thread.detach();
    }
 
    // Tancar el socket del servidor
    closesocket(server_socket);
    WSACleanup();
    return 0;
}