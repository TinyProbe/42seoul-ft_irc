#include "ResponsePool.h"

void ResponsePool::push(Response const &res) {
  new_responses_.push(res);
}

bool ResponsePool::pollResponse(Response &res) {
  if (new_responses_.empty()) {
    return false;
  }
  res = new_responses_.front();
  new_responses_.pop();
  return true;
}
