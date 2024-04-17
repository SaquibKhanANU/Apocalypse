#include <string>
#include "../client/Client.h"


class Lobby {
  public:
    Lobby(std::string lobby_name);
    ~Lobby();
    void start_lobby();
    void stop_lobby();
    std::string to_string() const;  

    void join_lobby(ClientInfo client_info);
    void leave_lobby();

    int get_num_players();
    int get_max_players();


  private:
    int max_players = 4;
    int num_players = 0;
    std::string lobby_name;
    std::vector<ClientInfo> clients; 
    void handle_client(int client_socket);
};