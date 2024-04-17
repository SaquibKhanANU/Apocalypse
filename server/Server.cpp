#include <iostream>
#include <unistd.h>
#include <thread>
#include "Server.h"

Server::Server(int port, int num_lobbies) : port(port), server_socket(0), num_lobbies(num_lobbies) {
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket\n";
        exit(1);
    }

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket\n";
        exit(1);
    }
}

Server::~Server() {
    stop_server();
}

void Server::start_server() {
    // Start listening for connections
    if (listen(server_socket, 5) < 0) {
        std::cerr << "Error listening on socket\n";
        exit(1);
    }

    std::cout << "Server started on port " << port << std::endl;

    for (int i = 0; i < num_lobbies; ++i) {
        lobbies.push_back(Lobby("Lobby " + std::to_string(i)));
    }

    while (true) {
        client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            std::cerr << "Error accepting connection\n";
            continue;
        }

        std::cout << "Client connected\n";

        std::string message = "Welcome to the server! \nType 'help' to see the list of commands\n" + handle_show_lobbies();
        send(client_socket, message.c_str(), message.size(), 0);

        std::thread client_thread(&Server::handle_client, this, client_socket);
        client_thread.detach();
    }
}

void Server::stop_server() {
    close(server_socket);
    std::cout << "Server stopped\n";
}

void Server::handle_client(int client_socket) {
    char buffer[1024];
    int bytes_received;

    Server::handle_initial_client_connection(client_socket);

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        if (bytes_received <= 0) {
            std::cerr << "Error receiving data from client\n";
            break;
        }
        buffer[bytes_received] = '\0';
        std::string command(buffer);
        std::string response = Server::handle_command(command, client_socket);
        send(client_socket, response.c_str(), response.length(), 0);
    }
    if (bytes_received == 0) {
        std::cout << "Client disconnected\n";
        for (auto it = clients.begin(); it != clients.end(); ++it) {
            if (it->client_socket == client_socket) {
                clients.erase(it);
                break;
            }
        }
    } else {
        std::cerr << "Error receiving data from client\n";
    }
    close(client_socket);
}

void Server::handle_initial_client_connection(int client_socket) {
    char buffer[1024];
    int bytes_received;

    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

    if (bytes_received <= 0) {
        std::cerr << "Error receiving username from client\n";
        close(client_socket);
        return;
    }

    buffer[bytes_received] = '\0';
    std::string username(buffer);
    std::string socket_str = std::to_string(client_socket);

    send(client_socket, ("Current user info: " + username + " on socket " + socket_str + "\n").c_str(),
                        ("Current user info: " + username + " on socket " + socket_str + "\n").length(), 0);

    ClientInfo clientInfo;
    clientInfo.client_socket = client_socket; 
    clientInfo.username = username;
    clients.push_back(clientInfo);
}

std::string Server::handle_command(const std::string& command, int client_socket) {
    std::istringstream iss(command);
    std::string cmd, param;
    iss >> cmd >> param;
    int index = 0;
    for (const auto& cmd_str : commands) {
        if (cmd == cmd_str || std::to_string(index + 1) == cmd){
            if (cmd == "help" || cmd == "1") {
                return get_commands();
            }
            else if (cmd == "show_lobbies" || cmd == "2") {
                return handle_show_lobbies();
            }
            else if (cmd == "join_lobby" || cmd == "3") {
                if (param.empty()) {
                    return "ERROR Missing lobby index\n";
                }
                int lobby_id = std::stoi(param);
                return handle_join_lobby(lobby_id, client_socket);
            }
            else if (cmd == "quit" || cmd == "4") {
                return handle_show_clients();
            }
        }
        index++;
    }
    
    return "ERROR Unknown command\nType 'help' to see the list of commands \n";
}

std::string Server::handle_show_lobbies() {
    std::ostringstream oss;
    oss << "Lobbies:\n";
    for (const auto& lobby : lobbies) {
        oss << lobby.to_string() << "\n";
    }
    return oss.str();
}

std::string Server::handle_show_clients() {
    std::ostringstream oss;
    oss << "Users: \n";
    int index = 1; // Initialize the index variable
    for (const auto& client : clients) {
        oss << index << ". " << client.username << "\n"; // Add a period after the index
        index++;
    }
    return oss.str();
}

std::string Server::handle_join_lobby(int lobby_id, int client_socket) {
    int index = 0;
    if (lobby_id >= 0 && lobby_id < lobbies.size()) {
        for (auto& lobby : lobbies) {
            index++;
            if (index != lobby_id) {
                continue;
            }
            if (lobby.get_num_players() < lobby.get_max_players()) {
                for (auto it = clients.begin(); it != clients.end(); ++it) {
                    if (it->client_socket == client_socket) {
                        ClientInfo& client_info = *it; // Access the element using the iterator directly
                        lobby.join_lobby(client_info);
                        break;
                    }
                }
                return "SUCCESS Joined lobby " + std::to_string(lobby_id) + "\n";
            }
        }
    } 
    return "ERROR Invalid lobby index\n";
}

std::string Server::get_commands() {
    std::ostringstream oss;
    oss << "Commands:\n";
    int index = 1;
    for (const auto& command : commands) {
        oss << index++ << ". " << command << "\n";
    }
    return oss.str(); // Return the string representation
}

std::string Server::handle_quit() {
    return "Goodbye!\n";
}

int main (int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_port> <num_lobbies>\n";
        return 1;
    }
    int port = std::atoi(argv[1]);
    int num_lobbies = std::atoi(argv[2]);
    Server server(port, num_lobbies);
    server.start_server();
    return 0;
}