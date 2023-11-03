#ifndef IRCSERV_SERVER_H_
#define IRCSERV_SERVER_H_

#include "Client.h"
#include <iostream>
#include <map>
#define MAX_BACKLOG 128

#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

namespace irc {

typedef std::map<int, Client> Connections;
typedef std::map<std::string, int> NickToSocket;

class Server {
 public:
  Server() {}
  ~Server() {}

  void setPort(std::string port);
  void setPassword(std::string password);
  int getSocket() const;
  Connections &getConnections();
  Client &getClient(int socket);
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