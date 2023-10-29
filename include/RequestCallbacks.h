#ifndef IRCSERV_REQUESTCALLBACKS_H_
#define IRCSERV_REQUESTCALLBACKS_H_

namespace irc {

typedef Response const &(*RequestCallback)(Request const &);

class RequestCallbacks {
 public:
  RequestCallbacks();
  ~RequestCallbacks();
  Response const &operator()(Request const &req);

 private:
  static Response const &unknown(Request const &req);
  static Response const &auth(Request const &req);
  static Response const &nickname(Request const &req);
  static Response const &username(Request const &req);
  static Response const &message(Request const &req);
  static Response const &join(Request const &req);
  static Response const &secede(Request const &req);
  static Response const &kick(Request const &req);
  static Response const &invite(Request const &req);
  static Response const &topic(Request const &req);
  static Response const &mode(Request const &req);

  std::vector<RequestCallback> callbacks_;
  Response response_;
};

} // namespace irc

#endif // IRCSERV_REQUESTCALLBACKS_H_
