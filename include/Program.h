#ifndef IRCSERV_PROGRAM_H_
#define IRCSERV_PROGRAM_H_

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
