#ifndef IRCSERV_SERVER_H_
#define IRCSERV_SERVER_H_

#include "Client.h"
#define MAX_BACKLOG 128

namespace irc {

using Connections = std::unordered_map<int, Client>;
using NickToSocket = std::unordered_map<std::string, int>;

class Server {
 public:
  Server() {}
  ~Server() {}

  void setPort(std::string port);
  void setPassword(std::string password);
  int getSocket() const;
  Connections &getConnections() const;
  Client &getClient(int socket) const;
  void standby();
  void preProcess();
  void disconnect(int socket);
  int accept();

 private:
  int port_;
  int server_socket_;
  std::string password_;
  Connections connections_;
  NickToSocket nick_to_socket_;
};

} // namespace irc

#endif // IRCSERV_SERVER_H_