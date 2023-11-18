#ifndef IRCSERV_REQUESTCALLBACK_H_
#define IRCSERV_REQUESTCALLBACK_H_

namespace irc {

typedef void (*RequestFunc)(Request const &, RequestPool &);

class RequestCallback {
 public:
  RequestCallback(Server &serv);
  ~RequestCallback() {}

  void operator()(Request const &req, RequestPool &requests);

 private:
  void unknown(Request const &req, RequestPool &requests);
  void pass(Request const &req, RequestPool &requests);
  void nick(Request const &req, RequestPool &requests);
  void user(Request const &req, RequestPool &requests);
  void privMsg(Request const &req, RequestPool &requests); // derive
  void join(Request const &req, RequestPool &requests); // derive
  void names(Request const &req, RequestPool &requests);
  void part(Request const &req, RequestPool &requests);
  void kick(Request const &req, RequestPool &requests);
  void invite(Request const &req, RequestPool &requests);
  void topic(Request const &req, RequestPool &requests);
  void mode(Request const &req, RequestPool &requests);

  Server &serv_;
  std::vector<RequestFunc> callbacks_;
};

} // namespace irc

#endif // IRCSERV_REQUESTCALLBACK_H_
