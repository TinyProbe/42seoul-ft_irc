#ifndef IRCSERV_PROGRAM_H_
#define IRCSERV_PROGRAM_H_

#include <iostream>
 #include <sys/socket.h>
 #include <sys/types.h>
#include "Request.h"
#include "Response.h"
#include "Server.h"
#include "EventPool.h"
#include "ResponsePool.h"
#include "RequestPool.h"
#include "Command.h"
#include <vector>
#include <unistd.h>
#include <cstring>

#define RECOGNIZE 1
#define JOIN 2
#define NICK 3
#define PRIVMSG 4
#define KICK 5
#define PART 6
#define QUIT 7
#define EXIT 8
#define TOPIC 9

namespace irc {

class Program {
 public:
  Program() {}
  ~Program() {}

  void run(int argc, char **argv);

 private:
  void init(int argc, char **argv);
  void loop();
  void request(struct kevent const &ev);
  void response(Request const &req);
  void perform(Response const &res);

  Server       server_;
  EventPool    events_;
  RequestPool  requests_;
  ResponsePool responses_;
};

} // namespace irc

#endif // IRCSERV_PROGRAM_H_
