#ifndef IRCSERV_RESPONSECALLBACKS_H_
#define IRCSERV_RESPONSECALLBACKS_H_

namespace irc {

typedef bool (*ResponseCallback)(Response const &);

class ResponseCallbacks {
 public:
  ResponseCallbacks();
  ~ResponseCallbacks();
  bool operator()(Response const &res);

 private:
  static bool unknown(Response const &res);
  static bool auth(Response const &res);
  static bool nickname(Response const &res);
  static bool username(Response const &res);
  static bool message(Response const &res);
  static bool join(Response const &res);
  static bool secede(Response const &res);
  static bool kick(Response const &res);
  static bool invite(Response const &res);
  static bool topic(Response const &res);
  static bool mode(Response const &res);

  std::vector<ResponseCallback> callbacks_;
};

} // namespace irc

#endif // IRCSERV_REQUESTCALLBACKS_H_
