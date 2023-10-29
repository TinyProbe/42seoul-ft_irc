#include "RequestCallbacks.h"

namespace irc {

RequestCallbacks::RequestCallbacks() {
  callbacks_.resize(Request::kCount);
  callbacks_[Request::kUnknown]  = RequestCallbacks::unknown;
  callbacks_[Request::kAuth]     = RequestCallbacks::auth;
  callbacks_[Request::kNickname] = RequestCallbacks::nickname;
  callbacks_[Request::kUsername] = RequestCallbacks::username;
  callbacks_[Request::kMessage]  = RequestCallbacks::message;
  callbacks_[Request::kJoin]     = RequestCallbacks::join;
  callbacks_[Request::kSecede]   = RequestCallbacks::secede;
  callbacks_[Request::kKick]     = RequestCallbacks::kick;
  callbacks_[Request::kInvite]   = RequestCallbacks::invite;
  callbacks_[Request::kTopic]    = RequestCallbacks::topic;
  callbacks_[Request::kMode]     = RequestCallbacks::mode;
}

RequestCallbacks::~RequestCallbacks() {
}

Response const &RequestCallbacks::operator()(Request const &req) {
  int code = req.getRequestCode();
  if (code < 0 || code >= Request::kCount) {
    throw std::runtime_error(
        std::string("RequestCallbacks(): invalid request code"));
  }
  return callbacks_[code](req);
}

Response const &RequestCallbacks::unknown(Request const &req) {
}

Response const &RequestCallbacks::auth(Request const &req) {
}

Response const &RequestCallbacks::nickname(Request const &req) {
}

Response const &RequestCallbacks::username(Request const &req) {
}

Response const &RequestCallbacks::message(Request const &req) {
}

Response const &RequestCallbacks::join(Request const &req) {
}

Response const &RequestCallbacks::secede(Request const &req) {
}

Response const &RequestCallbacks::kick(Request const &req) {
}

Response const &RequestCallbacks::invite(Request const &req) {
}

Response const &RequestCallbacks::topic(Request const &req) {
}

Response const &RequestCallbacks::mode(Request const &req) {
}

} // namespace irc
