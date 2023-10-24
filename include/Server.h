#ifndef SERVER_H
#define SERVER_H

#include "Client.h"
#define MAX_BACKLOG 128  
using Connections = std::unordered_map<int, Client>;
using Nicktoctl = std::unordered_map<string, int>;

namespace irc{

class Server {
 public:
  Server() {}
  ~Server() {}
  void	setPort(int port);
  void	setPassword(std::string Passowrd);
  void  standby(void);
  int		getSocket(void);
  void	preProcess(void);
  void	disconnect(int fd);
  int		accept(void);
  Connections getConnections(void);
  Client getClient(void);

  private:
	int port_number_;
	int server_socket_;
	std::string password_;
  Connections connections_;
  Nicktoctl nicktoctl_
};

}

#endif