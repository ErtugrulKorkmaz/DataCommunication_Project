#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Winsock kütüphanesini bağlar

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Winsock başlat
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // İstemci soketi oluştur
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Sunucu adresini ayarla
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Sunucuya bağlan
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Connected to server.\n");

    // Veri gönder ve cevap al
    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Yeni bir satır karakterini temizle
        buffer[strcspn(buffer, "\n")] = 0;

        // Veri gönder
        send(client_socket, buffer, strlen(buffer), 0);

        // Sunucudan cevap al
        memset(buffer, 0, BUFFER_SIZE);
        int recv_len = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (recv_len <= 0) {
            printf("Disconnected from server.\n");
            break;
        }

        printf("Server: %s\n", buffer);

        // Kullanıcıdan çıkış komutu almak
        if (strcmp(buffer, "Goodbye, Client!") == 0) {
            printf("Server has ended the connection.\n");
            break;
        }
    }

    // Kaynakları temizle
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
