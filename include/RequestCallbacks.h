#ifndef IRCSERV_REQUESTCALLBACK_H_
#define IRCSERV_REQUESTCALLBACK_H_

namespace irc {

typedef Response const &(*RequestFunc)(Request const &);

class RequestCallback {
 public:
  RequestCallback(Server &serv);
  ~RequestCallback() {}
  Response const &operator()(Request const &req);

 private:
  static Response const &unknown(Request const &req);
  static Response const &pass(Request const &req);
  static Response const &nick(Request const &req);
  static Response const &user(Request const &req);
  static Response const &privMsg(Request const &req);
  static Response const &join(Request const &req);
  static Response const &part(Request const &req);
  static Response const &kick(Request const &req);
  static Response const &invite(Request const &req);
  static Response const &topic(Request const &req);
  static Response const &mode(Request const &req);

  Server &serv_;
  std::vector<RequestFunc> callbacks_;

  Response response_;
};

} // namespace irc

#endif // IRCSERV_REQUESTCALLBACK_H_
