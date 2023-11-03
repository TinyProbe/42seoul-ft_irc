#include "RequestPool.h"

void irc::RequestPool::push(Request const &req) {
  new_requests_.push(req);
}

bool irc::RequestPool::pollRequest(Request &req) {
  if (new_requests_.empty()) {
    return false;
  }
  req = new_requests_.front();
  new_requests_.pop();
  return true;
}
