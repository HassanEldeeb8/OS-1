#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>

#define PORT 9090
#define BUFFER 1024

int main() {
    int server_fd, client_fd;
    sockaddr_in server_addr{}, client_addr{};
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    std::cout << "🔌 Server is running on port " << PORT << ". Waiting for client...\n";

    client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    std::cout << "✅ Client connected.\n";

    while (true) {
        memset(buffer, 0, BUFFER);
        read(client_fd, buffer, BUFFER);
        std::cout << "Client: " << buffer << std::endl;

        std::string reply;
        std::cout << "You: ";
        std::getline(std::cin, reply);
        send(client_fd, reply.c_str(), reply.length(), 0);
    }

    close(server_fd);
    return 0;
}
