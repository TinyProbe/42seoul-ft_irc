#ifndef IRCSERV_EVENTPOOL_H_
#define IRCSERV_EVENTPOOL_H_

#include <sys/event.h>
#include <vector>
#include <iostream>

namespace irc {

class EventPool {
 public:
  EventPool() : kqueue_(kqueue()) {
    if (kqueue_ == -1) {
      throw std::runtime_error(std::string("kqueue: ") +
                               std::string(strerror(errno)));
    }
  }
  ~EventPool() {}

  void changeEvent(int ident, int filter, int flags);
  bool pollEvent(struct kevent &ev);

 private:
  int kqueue_;
  std::vector<struct kevent> new_events_;
};

} // namespace irc

#endif // IRCSERV_EVENTPOOL_H_
