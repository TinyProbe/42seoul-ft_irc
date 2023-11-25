#include "common.h"

namespace irc {

void RequestPool::push(Request const &req) { new_requests_.push(req); }

bool RequestPool::pollRequest(Request &req) {
  if (new_requests_.empty()) {
    return false;
  }
  req = new_requests_.front();
  new_requests_.pop();
  return true;
}

size_t RequestPool::getSize() const { return new_requests_.size(); }

} // namespace irc
