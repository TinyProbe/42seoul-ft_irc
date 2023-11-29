/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventPool.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkong <tkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 22:17:43 by tkong             #+#    #+#             */
/*   Updated: 2023/11/29 22:17:43 by tkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_EVENTPOOL_H_
#define IRCSERV_EVENTPOOL_H_

#include <sys/event.h>
#include <sys/errno.h>

#include <vector>
#include <string>
#include <stdexcept>

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

  size_t getCapacity() const;
  void setCapacity(size_t cap);

  void changeEvent(int ident, int filter, int flags);
  bool pollEvent(struct kevent &ev);

 private:
  int kqueue_;
  std::vector<struct kevent> new_events_;
};

} // namespace irc

#endif // IRCSERV_EVENTPOOL_H_
