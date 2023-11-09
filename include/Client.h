#ifndef IRCSERV_CLIENT_H_
#define IRCSERV_CLIENT_H_

#define MAX_BUF 1024
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include "Request.h"

namespace irc {

class Client {
 public:
  Client() : write_(false) {}
  ~Client() {}
  Client &operator=(const Client &a);

  std::string getNickname() const ;
  void receive(int socket);
  bool canRequest() const ; 
  Request createRequest();
  void setWrite(bool value);
  std::string getPassword() const;
  bool canWrite() const ;
  void certification();
  void setNickname(std::string nick);

 private:
  std::string nickname_;
  std::string usrname_;
  bool write_;
  std::string receive_;
  std::string password_;
  int socket_;
};

} // namespace irc

#endif // IRCSERV_CLIENT_H_