#include <string>

struct ClientInfo {
  int client_socket;
  std::string username;
};

#ifndef CLIENT_H
#define CLIENT_H


class Client {
public:
  Client(std::string username);
  ~Client();
  void connect_to_server();
  void close_connection();
  void send_message(std::string message);
  std::string receive_response();

private:
  std::string server_ip;
  int server_port;
  int client_socket;
  struct sockaddr_in server_addr;

  std::string username;

};


#endif // CLIENT_H