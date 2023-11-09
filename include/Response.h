#ifndef IRCSERV_RESPONSE_H_
#define IRCSERV_RESPONSE_H_

#include <iostream>

namespace irc {

class Response {
 public:
  Response(std::string receive, int type, int socket)
  : response_(receive), type_(type), socket_(socket) {}
  Response() {}
  ~Response() {}

  int getResponseCode() const {
	return (1);
  }
  std::string getResponse() const {
    return response_;
  }
  std::string response_;
  int type_;
  int socket_;
};

} // namespace irc

#endif // IRCSERV_RESPONSE_H_
