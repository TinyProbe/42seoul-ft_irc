#include "RequestCallback.h"

namespace irc {

RequestCallback::RequestCallback(Server &serv)
    : serv_(serv) {
  callbacks_.resize(Request::kCount);
  callbacks_[Request::kUnknown] = RequestCallback::unknown;
  callbacks_[Request::kPass]    = RequestCallback::pass;
  callbacks_[Request::kNick]    = RequestCallback::nick;
  callbacks_[Request::kUser]    = RequestCallback::user;
  callbacks_[Request::kPrivMsg] = RequestCallback::privMsg;
  callbacks_[Request::kJoin]    = RequestCallback::join;
  callbacks_[Request::kPart]    = RequestCallback::part;
  callbacks_[Request::kKick]    = RequestCallback::kick;
  callbacks_[Request::kInvite]  = RequestCallback::invite;
  callbacks_[Request::kTopic]   = RequestCallback::topic;
  callbacks_[Request::kMode]    = RequestCallback::mode;
}

std::vector<Response> const &RequestCallback::operator()(Request const &req) {
  int code = req.getRequestCode();
  if (code < 0 || code >= Request::kCount) {
    throw std::runtime_error(
        std::string("RequestCallback(): invalid request code"));
  }
  return callbacks_[code](req);
}

std::vector<Response> const &RequestCallback::unknown(Request const &req) {
}

std::vector<Response> const &RequestCallback::pass(Request const &req) {
}

std::vector<Response> const &RequestCallback::nick(Request const &req) {
}

std::vector<Response> const &RequestCallback::user(Request const &req) {
}

std::vector<Response> const &RequestCallback::privMsg(Request const &req) {
}

std::vector<Response> const &RequestCallback::join(Request const &req) {
}

std::vector<Response> const &RequestCallback::part(Request const &req) {
}

std::vector<Response> const &RequestCallback::kick(Request const &req) {
}

std::vector<Response> const &RequestCallback::invite(Request const &req) {
}

std::vector<Response> const &RequestCallback::topic(Request const &req) {
}

std::vector<Response> const &RequestCallback::mode(Request const &req) {
}

} // namespace irc
