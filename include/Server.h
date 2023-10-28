#ifndef IRCSERV_SERVER_H_
#define IRCSERV_SERVER_H_

#include "Client.h"

namespace irc {

using Connections = std::unordered_map<int, Client>;
using NickToSocket = std::unordered_map<std::string, int>;

class Server {
 public:
  Server() : port_(-1), serv_sock_(-1) {}
  ~Server() {}

  int getSocket() const;
  Connections &getConnections() const;
  Client &getClient(int socket) const;
  Client &getClient(std::string const &nick) const;
  void setPort(int port);
  void setPassword(std::string const &password);
  bool verify(std::string const &password) const;

  void standby();
  void preProcess();
  void disconnect(int socket);
  void disconnect(std::string const &nick);
  int accept();

 private:
  int port_;
  int serv_sock_;
  std::string password_;
  Connections connections_;
  NickToSocket nick_to_sock_;
};

} // namespace irc

#endif // IRCSERV_SERVER_H_
