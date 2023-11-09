#ifndef IRCSERV_REQUEST_H_
#define IRCSERV_REQUEST_H_

#include <iostream>

namespace irc {

class Request {
 public:
  Request(std::string receive, std::string nickname, std::string usrname, int socket) 
  : type_(receive), nickname_(nickname), usrname_(usrname), socket_(socket) {
  }
  Request() {}
  ~Request() {}

  int getRequestCode() const ;
  int getSocket() const ;
  std::string getNickname() const ;
  std::string getUsrname() const ;
  std::string getType() const;
 private:
  std::string type_;
  std::string nickname_;
  std::string usrname_;
  int socket_;
};

} // namespace irc

#endif // IRCSERV_REQUEST_H_
