#include <iostream>
#include <unistd.h>
#include "Lobby.h"

Lobby::Lobby(std::string lobby_name) : lobby_name(lobby_name) {
    //ctor
}

Lobby::~Lobby() {
    //dtor
}

void Lobby::start_lobby() {
    // Start the lobby
}

void Lobby::stop_lobby() {
    // Stop the lobby
}

void Lobby::handle_client(int client_socket) {
    // Handle the client
}

void Lobby::join_lobby(ClientInfo client_info) {
    // Join the lobby
    clients.push_back(client_info);
}

void Lobby::leave_lobby() {
    // Leave the lobby
}

int Lobby::get_num_players() {
    return num_players;
}

int Lobby::get_max_players() {
    return max_players;
}

std::string Lobby::to_string() const {
    std::ostringstream oss;
    oss << lobby_name << ": " << num_players << "/" << max_players;
    return oss.str(); // Return the string representation
}
