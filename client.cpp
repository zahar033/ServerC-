#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define DEFAULT_PORT 1505
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    std::string message;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Client Error: Failed to create socket." << std::endl;
        return EXIT_FAILURE;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");  // Localhost

    if (connect(sock, reinterpret_cast<struct sockaddr*>(&server_address), sizeof(server_address)) < 0) {
        std::cerr << "Client Error: Connection failed." << std::endl;
        close(sock);
        return EXIT_FAILURE;
    }

    std::cout << "Connected to the server.\n";

    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);

        if (message.empty()) continue;

        send(sock, message.c_str(), message.size(), 0);

        if (message == "#") {
            std::cout << "Closing connection." << std::endl;
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            std::cerr << "Client Error: Failed to receive message." << std::endl;
            break;
        } else if (bytes_received == 0) {
            std::cout << "Server disconnected." << std::endl;
            break;
        }

        std::cout << "Server: " << buffer << std::endl;
    }

    close(sock);
    return 0;
}
