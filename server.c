#include <stdio.h>
#include <winsock2.h> 
#include <windows.h> 
#include <string.h>  

#pragma comment(lib, "ws2_32.lib")  

#define PORT 8080
#define BUFFER_SIZE 1024


const char *weather_data[7] = {
    "Pazartesi: Gunesli, 23 C",
    "Sali: Bulutlu, 17 C",
    "Carsamba: Yagmurlu, 15 C",
    "Persembe: Gunesli, 18 C",
    "Cuma: Ruzgarli, 7 C",
    "Cumartesi: Yagmurlu, 10 C",
    "Pazar: Bulutlu, 17 C"
};


void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];   
    char response[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        int recv_len = recv(client_socket, buffer, BUFFER_SIZE, 0);  
        if (recv_len <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Client: %s\n", buffer);

        
        int day = atoi(buffer);    
        if (day >= 1 && day <= 7) {
            snprintf(response, BUFFER_SIZE, " %d Numarali Gunun Hava Durumu : %s", day, weather_data[day - 1]);
        } else {
            snprintf(response, BUFFER_SIZE, "Invalid input. Please enter a number between 1 and 7.");
        }

        
        send(client_socket, response, strlen(response), 0);   
    }

    closesocket(client_socket);   
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

   
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

  
    listen(server_socket, 3);
    printf("Server is listening on port %d...\n", PORT);

    
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed. Error Code: %d\n", WSAGetLastError());
            continue;
        }

        printf("Client connected.\n");
        
        DWORD thread_id;
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)handle_client, (LPVOID)client_socket, 0, &thread_id);
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}
