#ifndef IRCSERV_RESPONSECALLBACK_H_
#define IRCSERV_RESPONSECALLBACK_H_

namespace irc {

typedef bool (*ResponseFunc)(Response const &);

class ResponseCallback {
 public:
  ResponseCallback(Server &serv);
  ~ResponseCallback() {}
  bool operator()(Response const &res);

 private:
  static bool unknown(Response const &res);
  static bool pass(Response const &res);
  static bool nick(Response const &res);
  static bool user(Response const &res);
  static bool privMsg(Response const &res);
  static bool join(Response const &res);
  static bool part(Response const &res);
  static bool kick(Response const &res);
  static bool invite(Response const &res);
  static bool topic(Response const &res);
  static bool mode(Response const &res);

  Server &serv_;
  std::vector<ResponseFunc> callbacks_;
};

} // namespace irc

#endif // IRCSERV_RESPONSECALLBACK_H_
