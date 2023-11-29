/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Program.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkong <tkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 22:17:39 by tkong             #+#    #+#             */
/*   Updated: 2023/11/29 22:17:39 by tkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_PROGRAM_H_
#define IRCSERV_PROGRAM_H_

#include <string>

#include "Server.h"
#include "EventPool.h"
#include "RequestPool.h"

struct kevent;

namespace irc {

class Request;

class Program {
 public:
  Program() {}
  ~Program() {}

  void run(int argc, char **argv);

 private:
  void init(char **argv);
  void loop();
  void request(struct kevent const &ev);
  void response(Request const &req);

  Server       serv_;
  EventPool    events_;
  RequestPool  requests_;
};

} // namespace irc

#endif // IRCSERV_PROGRAM_H_
