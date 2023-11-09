#ifndef IRCSERV_SERVER_H_
#define IRCSERV_SERVER_H_

#include "Client.h"
#include <iostream>
#include <map>
#define MAX_BACKLOG 128

#include <netinet/in.h>
#include <fcntl.h>
#include "Channel.h"
#include <unistd.h>

namespace irc {

typedef std::map<int, Client> Connections;
typedef std::map<std::string, int> NickToSocket;
typedef std::map<std::string, Channel> SevChannel;

class Server {
 public:
  Server() {}
  ~Server() {}

  void setPort(std::string port);
  void setPassword(std::string password);
  int getSocket() const;
  std::string getPassword() const;
  Connections &getConnections();
  Client &getClient(int socket);
  void standby();
  void preProcess();
  void disconnect(int socket);
  int accept();
  int samename(std::string nickname) const;
  bool setChannel(std::string name, std::string password, std::string nick);
  Channel &getChannel(std::string name);

 private:
  int port_;
  int server_socket_;
  std::string password_;
  Connections connections_;
  NickToSocket nick_to_socket_;
  SevChannel sev_channel_;
};

} // namespace irc

#endif // IRCSERV_SERVER_H_