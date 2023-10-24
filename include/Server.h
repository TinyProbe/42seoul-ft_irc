#ifndef IRCSERV_SERVER_H_
#define IRCSERV_SERVER_H_

#include "Client.h"
#define MAX_BACKLOG 128

namespace irc {

using Connections = std::unordered_map<int, Client>;
using NickToSocket = std::unordered_map<string, int>;

class Server {
 public:
  Server() {}
  ~Server() {}

  void setPort(int port);
  void setPassword(std::string Passowrd);
  void standby(void);
  int getSocket(void) const;
  void preProcess(void);
  void disconnect(int socket);
  int accept(void);
  Connections &getConnections(void) const;
  Client &getClient(int socket) const;

 private:
  int port_number_;
  int server_socket_;
  std::string password_;
  Connections connections_;
  NickToSocket nick_to_socket_;
};

} // namespace irc

#endif // IRCSERV_SERVER_H_