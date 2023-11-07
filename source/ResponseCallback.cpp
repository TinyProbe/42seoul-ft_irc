#include "ResponseCallback.h"

namespace irc {

ResponseCallback::ResponseCallback(Server &serv)
    : serv_(serv) {
  callbacks_.resize(Response::kCount);
  callbacks_[Response::kUnknown] = ResponseCallback::unknown;
  callbacks_[Response::kPass]    = ResponseCallback::pass;
  callbacks_[Response::kNick]    = ResponseCallback::nick;
  callbacks_[Response::kUser]    = ResponseCallback::user;
  callbacks_[Response::kPrivMsg] = ResponseCallback::privMsg;
  callbacks_[Response::kJoin]    = ResponseCallback::join;
  callbacks_[Response::kPart]    = ResponseCallback::part;
  callbacks_[Response::kKick]    = ResponseCallback::kick;
  callbacks_[Response::kInvite]  = ResponseCallback::invite;
  callbacks_[Response::kTopic]   = ResponseCallback::topic;
  callbacks_[Response::kMode]    = ResponseCallback::mode;
}

bool ResponseCallback::operator()(Response const &res) {
  int code = res.getResponseCode();
  if (code < 0 || code >= Response::kCount) {
    throw std::runtime_error(
        std::string("ResponseCallback(): invalid response code"));
  }
  return callbacks_[code](res);
}

bool ResponseCallback::unknown(Response const &res) {
}

bool ResponseCallback::pass(Response const &res) {
}

bool ResponseCallback::nick(Response const &res) {
}

bool ResponseCallback::user(Response const &res) {
}

bool ResponseCallback::privMsg(Response const &res) {
}

bool ResponseCallback::join(Response const &res) {
}

bool ResponseCallback::part(Response const &res) {
}

bool ResponseCallback::kick(Response const &res) {
}

bool ResponseCallback::invite(Response const &res) {
}

bool ResponseCallback::topic(Response const &res) {
}

bool ResponseCallback::mode(Response const &res) {
}

} // namespace irc
