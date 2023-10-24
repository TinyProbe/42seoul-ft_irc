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

  void setPort(int port) { prot_ = port; }
  void setPassword(std::string password) { password_ = password; }
  int getSocket(void) const { return server_socket_; }
  Connections &getConnections(void) const { return connections_; }
  Client &getClient(int socket) const { return connections_[socket]; }
  
  void standby(void);
  void preProcess(void);
  void disconnect(int socket);
  int accept(void);

 private:
  int port_;
  int server_socket_;
  std::string password_;
  Connections connections_;
  NickToSocket nick_to_socket_;
};

} // namespace irc

#endif // IRCSERV_SERVER_H_