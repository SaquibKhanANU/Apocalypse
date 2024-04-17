#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "Client.h"


Client::Client(std::string username) : server_ip("127.0.0.1"), server_port(8080), username(username), client_socket(0) {
}

Client::~Client() {
    //dtor
}

void Client::connect_to_server() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error creating socket\n";
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr); // Convert string to C-style string with c_str()

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error connecting to server\n";
        return;
    }

    std::cout << "Connected to server\n";
}

void Client::send_message(std::string message) {
    if (send(client_socket, message.c_str(), message.size(), 0) < 0) {
        std::cerr << "Error sending message to server\n";
        return;
    }

    // std::cout << "Message sent to server\n";
}

std::string Client::receive_response() {
    char buffer[1024];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        std::cerr << "Error receiving response from server\n";
        return "";
    }

    buffer[bytes_received] = '\0';
    std::string response(buffer); 
    std::cout << "\n\t Response: \n" << response << std::endl;
    return response;
}

void Client::close_connection() {
    close(client_socket);
    std::cout << "Connection closed\n";
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <username>\n";
        return 1;
    }

    std::string username = argv[1];

    Client client(username);
    client.connect_to_server();
    client.receive_response();

    client.send_message(username.c_str());
    client.receive_response();

    while (true) {
        std::string input;
        std::cout << "Message: ";
        std::getline(std::cin, input);

        if (input.empty()) {
            continue;
        }

        client.send_message(input.c_str());
        client.receive_response();
    }

    client.close_connection();
    return 0;
}