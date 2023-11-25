#include "common.h"

namespace irc {

size_t EventPool::getCapacity() const {
  return new_events_.size();
}

void EventPool::setCapacity(size_t cap) {
  new_events_.resize(cap);
}

void EventPool::changeEvent(int ident, int filter, int flags) {
  static struct kevent ev;
  EV_SET(&ev, ident, filter, flags, 0, 0, NULL);
  if (kevent(kqueue_, &ev, 1, NULL, 0, NULL) == -1) {
    throw std::runtime_error(std::string("kevent: ") +
                             std::string(strerror(errno)));
  }
}

bool EventPool::pollEvent(struct kevent &ev) {
  static int len;
  if (len == 0) {
    len = kevent(kqueue_, NULL, 0, &new_events_[0], new_events_.size(), NULL);
    if (len == -1) {
      throw std::runtime_error(std::string("kevent: ") +
                               std::string(strerror(errno)));
    }
    return false;
  }
  memcpy(&ev, &new_events_[--len], sizeof(struct kevent));
  return true;
}

} // namespace irc
