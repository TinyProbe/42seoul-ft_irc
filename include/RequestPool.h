/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestPool.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migo <migo@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 22:17:45 by migo              #+#    #+#             */
/*   Updated: 2023/11/29 22:17:45 by migo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_REQUESTPOOL_H_
#define IRCSERV_REQUESTPOOL_H_

#include <queue>

#include "Request.h"

namespace irc {

class RequestPool {
 public:
  RequestPool() {}
  ~RequestPool() {}

  size_t getSize() const;

  void push(Request const &req);
  bool pollRequest(Request &req);

 private:
  std::queue<Request> new_requests_;
};

} // namespace irc

#endif // IRCSERV_REQUESTPOOL_H_
