#include "ResponseCallbacks.h"

namespace irc {

ResponseCallbacks::ResponseCallbacks() {
  callbacks_.resize(Response::kCount);
  callbacks_[Response::kUnknown]  = ResponseCallbacks::unknown;
  callbacks_[Response::kAuth]     = ResponseCallbacks::auth;
  callbacks_[Response::kNickname] = ResponseCallbacks::nickname;
  callbacks_[Response::kUsername] = ResponseCallbacks::username;
  callbacks_[Response::kMessage]  = ResponseCallbacks::message;
  callbacks_[Response::kJoin]     = ResponseCallbacks::join;
  callbacks_[Response::kSecede]   = ResponseCallbacks::secede;
  callbacks_[Response::kKick]     = ResponseCallbacks::kick;
  callbacks_[Response::kInvite]   = ResponseCallbacks::invite;
  callbacks_[Response::kTopic]    = ResponseCallbacks::topic;
  callbacks_[Response::kMode]     = ResponseCallbacks::mode;
}

ResponseCallbacks::~ResponseCallbacks() {
}

bool ResponseCallbacks::operator()(Response const &res) {
  int code = req.getResponseCode();
  if (code < 0 || code >= Response::kCount) {
    throw std::runtime_error(
        std::string("ResponseCallbacks(): invalid response code"));
  }
  return callbacks_[code](res);
}

bool ResponseCallbacks::unknown(Response const &res) {
}

bool ResponseCallbacks::auth(Response const &res) {
}

bool ResponseCallbacks::nickname(Response const &res) {
}

bool ResponseCallbacks::username(Response const &res) {
}

bool ResponseCallbacks::message(Response const &res) {
}

bool ResponseCallbacks::join(Response const &res) {
}

bool ResponseCallbacks::secede(Response const &res) {
}

bool ResponseCallbacks::kick(Response const &res) {
}

bool ResponseCallbacks::invite(Response const &res) {
}

bool ResponseCallbacks::topic(Response const &res) {
}

bool ResponseCallbacks::mode(Response const &res) {
}

} // namespace irc
