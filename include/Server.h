#ifndef IRCSERV_SERVER_H_
#define IRCSERV_SERVER_H_

#include "Client.h"

namespace irc {

typedef std::unordered_map<int, Client> Connections;
typedef std::unordered_map<std::string, int> NickToSock;

class Server {
 public:
  Server() : serv_sock_(-1), port_(-1);
  ~Server();

  int getSocket() const;
  Connections &getConnections() const;
  Client &getClient(int socket) const;
  Client &getClient(std::string const &nick) const;
  void setPort(int port);
  void setPassword(std::string const &password);
  bool verify(std::string const &password) const;

  void standby();
  int accept();
  void preProcess();
  void disconnect(int socket);
  void disconnect(std::string const &nick);

  Response const &response(Request const &req);
  bool perform(Response const &res);


 private:
  int         serv_sock_;
  int         port_;
  std::string password_;

  Connections connections_;
  NickToSock  nick_to_sock_;

  RequestCallbacks request_callbacks_;
  ResponseCallbacks response_callbacks_;
};

} // namespace irc

#endif // IRCSERV_SERVER_H_
