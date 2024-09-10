#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define ERROR_S "SERVER ERROR:"
#define DEFAULT_PORT 1505
#define BUFFER_SIZE 1024

void* handle_client(void* client_sock_ptr) {
    int client_sock = *(int*)client_sock_ptr;
    char buffer[BUFFER_SIZE];
    std::string server_message = "=> Server Ok\n";

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            std::cerr << ERROR_S << " Error receiving message." << std::endl;
            break;
        } else if (bytes_received == 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        std::cout << "Client: " << buffer << std::endl;

        send(client_sock, server_message.c_str(), server_message.size(), 0);

        if (strncmp(buffer, "#", 1) == 0) {
            std::cout << "Connection closed by client." << std::endl;
            break;
        }
    }

    close(client_sock);
    return nullptr;
}

int main() {
    int client_sock, server_sock;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    pthread_t thread_id;

    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock < 0) {
        std::cerr << ERROR_S << " Failed to create socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Socket created successfully.\n";

    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(client_sock, reinterpret_cast<struct sockaddr*>(&server_address), sizeof(server_address));
    if(ret < 0){
        std::cerr << ERROR_S << " Bind failed." << std::endl;
        close(client_sock);
        return EXIT_FAILURE;
    }

    std::cout << "Bind successful. Listening for clients...\n";
    if (listen(client_sock, 5) < 0) {
        std::cerr << ERROR_S << " Listen failed." << std::endl;
        close(client_sock);
        return EXIT_FAILURE;
    }

    while (true) {
        server_sock = accept(client_sock, reinterpret_cast<struct sockaddr*>(&client_address), &client_address_len);
        if(server_sock < 0) {
            std::cerr << ERROR_S << " Can't accept client." << std::endl;
            close(client_sock);
            return EXIT_FAILURE;
        }

        std::cout << "Client connected. Starting new thread...\n";

        if (pthread_create(&thread_id, nullptr, handle_client, &server_sock) != 0) {
            std::cerr << ERROR_S << " Failed to create thread." << std::endl;
        }

        pthread_detach(thread_id);
    }

    close(client_sock);
    return 0;
}
