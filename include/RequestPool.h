#ifndef IRCSERV_REQUESTPOOL_H_
#define IRCSERV_REQUESTPOOL_H_

#include <queue>
#include "Request.h"

namespace irc {

class RequestPool {
 public:
  RequestPool() {}
  ~RequestPool() {}

  void push(Request const &req);
  bool pollRequest(Request &req);

 private:
  std::queue<Request> new_requests_;
};

} // namespace irc

#endif // IRCSERV_REQUESTPOOL_H_
