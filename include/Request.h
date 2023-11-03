#ifndef IRCSERV_REQUEST_H_
#define IRCSERV_REQUEST_H_

#include <iostream>

namespace irc {

class Request {
 public:
  Request(std::string receive) : type(receive) {}
  Request() {}
  ~Request() {}

  int getRequestCode() const {
    if (type[0] == 'U') { return 1; }
    if (type[0] ==  'J') { return 2; }
    return 0;
  }
  int getSocket() const {
    return socket;
  }
 private:
  std::string type;
  int socket;
};

} // namespace irc

#endif // IRCSERV_REQUEST_H_
