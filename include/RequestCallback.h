#ifndef IRCSERV_REQUESTCALLBACK_H_
#define IRCSERV_REQUESTCALLBACK_H_

namespace irc {

typedef std::vector<Response> const &(*RequestFunc)(Request const &);

class RequestCallback {
 public:
  RequestCallback(Server &serv);
  ~RequestCallback() {}
  std::vector<Response> const &operator()(Request const &req);

 private:
  static std::vector<Response> const &unknown(Request const &req);
  static std::vector<Response> const &pass(Request const &req);
  static std::vector<Response> const &nick(Request const &req);
  static std::vector<Response> const &user(Request const &req);
  static std::vector<Response> const &privMsg(Request const &req);
  static std::vector<Response> const &join(Request const &req);
  static std::vector<Response> const &part(Request const &req);
  static std::vector<Response> const &kick(Request const &req);
  static std::vector<Response> const &invite(Request const &req);
  static std::vector<Response> const &topic(Request const &req);
  static std::vector<Response> const &mode(Request const &req);

  Server &serv_;

  std::vector<RequestFunc> callbacks_;

  static std::vector<Response> ress_;
};

} // namespace irc

#endif // IRCSERV_REQUESTCALLBACK_H_
