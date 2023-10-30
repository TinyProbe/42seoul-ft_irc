#ifndef IRCSERV_CLIENT_H_
#define IRCSERV_CLIENT_H_

#define MAX_BACKLOG 1024

namespace irc {

class Client {
 public:
  Client() : write_(false) {}
  ~Client() {}

 private:

};

} // namespace irc

#endif // IRCSERV_CLIENT_H_
