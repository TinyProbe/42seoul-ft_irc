#ifndef IRCSERV_RESPONSEPOOL_H_
#define IRCSERV_RESPONSEPOOL_H_

#include <queue>
#include "Response.h"

namespace irc {

class ResponsePool {
 public:
  ResponsePool() {}
  ~ResponsePool() {}

  void push(Response const &res);
  bool pollResponse(Response &res);

 private:
  std::queue<Response> new_responses_;
};

} // namespace irc

#endif // IRCSERV_RESPONSEPOOL_H_
