#ifndef IRCSERV_RESPONSE_H_
#define IRCSERV_RESPONSE_H_

#include <iostream>

namespace irc {

class Response {
 public:
  Response(std::string receive) : type(receive) {}
  Response() {}
  ~Response() {}

  int getResponseCode() const {
	return (1);
  }
  std::string type;
};

} // namespace irc

#endif // IRCSERV_RESPONSE_H_
