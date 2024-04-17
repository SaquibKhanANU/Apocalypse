#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <sstream> 
#include "Lobby.cpp"

class Server {
  public:
    Server(int port, int num_lobbies);
    ~Server();
    void start_server();
    void stop_server();


  private:
    int port;
    int server_socket;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    int num_lobbies;
    std::vector<Lobby> lobbies;

    std::vector<ClientInfo> clients; 
    void handle_initial_client_connection(int client_socket);
    void handle_client(int client_socket);

    std::vector<std::string> commands = {"help", "show_lobbies", "join_lobby", "quit"};
    std::string handle_command(const std::string& command, int client_socket);
    std::string get_commands();
    std::string handle_show_lobbies();
    std::string handle_join_lobby(int lobby_id, int client_socket);
    std::string handle_quit();
    std::string handle_show_clients();
};