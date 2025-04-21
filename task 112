#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

#define PORT 9090
#define BUFFER 1024

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};
    char buffer[BUFFER] = {0};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);  // localhost

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    std::cout << "✅ Connected to server.\n";

    while (true) {
        std::string message;
        std::cout << "You: ";
        std::getline(std::cin, message);
        send(sock, message.c_str(), message.length(), 0);

        memset(buffer, 0, BUFFER);
        read(sock, buffer, BUFFER);
        std::cout << "Server: " << buffer << std::endl;
    }

    close(sock);
    return 0;
}
