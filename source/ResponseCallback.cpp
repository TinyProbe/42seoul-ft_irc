#include "ResponseCallback.h"

namespace irc {

static bool send_(Server &serv,
                  Client const &client,
                  std::string const &response_msg,
                  std::string const &call_stack) {
  if (send(client.getSocket(),
           response_msg.c_str(),
           response_msg.size(),
           MSG_DONTWAIT | MSG_NOSIGNAL) == -1) {
    if (errno == EAGAIN) {
      return false;
    } else {
      std::cerr << call_stack + std::string(strerror(errno)) << std::endl;
      serv.disconnect(client.getSocket());
    }
  }
  return true;
}

static void auth(Server const &serv, Client &client) {
  if (!client.getAuth() && client.getNick() != "*" && client.getUser() != "*") {
    if (serv_.verify(client.getPassword())) {
      // RPL_WELCOME
      client.setAuth(true);
      std::string response_msg =
          std::string(":") + serv.getHost() + " " +
          "001" + client.getNick() + " " +
          ":Welcome to the Internet Relay Network " +
          client.getNick() + "!" + client.getUser() + "@" +
          client.getHost() + "\r\n";
      send_(serv, client, response_msg, "perform: auth: ");
    }
  }
}

static bool nickCheck(std::string const &nick) {
  if (nick.size() < 1 || nick.size() > 9) {
    return false;
  } else if (!isalpha(nick[0]) &&
      std::string("[]\\`_^{|}").find(nick[0]) == std::string::npos) {
    return false;
  }
  for (int i = 1; i < nick.size(); ++i) {
    if (!isalpha(nick[i]) && !isdigit(nick[i]) &&
        std::string("[]\\`_^{|}-").find(nick[i]) == std::string::npos) {
      return false;
    }
  }
  return true;
}

static bool targetCheck(Server const &serv, std::string const &target) {
  if (target[0] == '#') {
    ChannelMap const &channel_map = serv.getChannelMap();
    if (channel_map.find(target) == channel_map.end()) { return false; }
  } else {
    Connection const &connection = serv.getConnection();
    if (connection.find(target) == connection.end()) { return false; }
  }
  return true;
}

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
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(res.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    std::string response_msg =
        std::string(":") + serv_.getHost() + " " +
        "421" + " " +
        client.getNick() + " " +
        res.getCommand() + " " +
        ":Unknown command" +
        "\r\n";
    return send_(serv_, client, response_msg, "perform: unknown: ");
  }
  return true;
}

bool ResponseCallback::pass(Response const &res) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(res.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    std::vector<std::string> const &param = res.getParam();
    std::string response_msg =
        std::string(":") + client.getNick() + "!" +
        client.getUser() + "@" + client.getHost() + " ";
    if (param.size() < 1) { // ERR_NEEDMOREPARAM
      response_msg += "461 ";
      response_msg += res.getCommand() + " ";
      response_msg += ":Not enough parameters\r\n";
    } else if (client.getAuth()) { // ERR_ALREADYREGISTERED
      response_msg += "462 ";
      response_msg += ":Unauthorized command (already registered)\r\n";
    } else {
      client.setPassword(param[0]);
      auth(serv_, client);
      return true;
    }
    return send_(serv_, client, response_msg, "perform: pass: ");
  }
  return true;
}

bool ResponseCallback::nick(Response const &res) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(res.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    std::vector<std::string> const &param = res.getParam();
    std::string response_msg =
        std::string(":") + client.getNick() + "!" +
        client.getUser() + "@" + client.getHost() + " ";
    if (param.size() < 1) { // ERR_NONICKNAMEGIVEN
      response_msg += "431 ";
      response_msg += ":No nickname given\r\n";
    } else if (!nickCheck(param[0])) { // ERR_ERRONEUSNICKNAME
      response_msg += "432 ";
      response_msg += param[0] + " ";
      response_msg += ":Erroneous nickname\r\n";
    } else if (serv_.getNickToSock().find(param[0]) !=
               serv_.getNickToSock().end()) { // ERR_NICKNAMEINUSE
      response_msg += "433 ";
      response_msg += param[0] + " ";
      response_msg += ":Nickname is already in use\r\n";
    // } else if () { // ERR_NICKCOLLISION
    // } else if () { // ERR_UNAVAILRESOURCE
    // } else if () { // ERR_RESTRICTED
    } else {
      client.setNick(param[0]);
      auth(serv_, client);
      return true;
    }
    return send_(serv_, client, response_msg, "perform: nick: ");
  }
  return true;
}

bool ResponseCallback::user(Response const &res) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(res.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    std::vector<std::string> const &param = res.getParam();
    std::string response_msg =
        std::string(":") + client.getNick() + "!" +
        client.getUser() + "@" + client.getHost() + " ";
    if (param.size() < 1) { // ERR_NEEDMOREPARAM
      response_msg += "461 ";
      response_msg += res.getCommand() + " ";
      response_msg += ":Not enough parameters\r\n";
    } else if (client.getAuth()) { // ERR_ALREADYREGISTERED
      response_msg += "462 ";
      response_msg += ":Unauthorized command (already registered)\r\n";
    } else {
      client.setUser(param[0]);
      auth(serv_, client);
      return true;
    }
    return send_(serv_, client, response_msg, "perform: user: ");
  }
  return true;
}

bool ResponseCallback::privMsg(Response const &res) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(res.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    std::vector<std::string> const &param = res.getParam();
    std::string response_msg =
        std::string(":") + client.getNick() + "!" +
        client.getUser() + "@" + client.getHost() + " ";
    if (param.size() < 1) { // ERR_NORECIPIENT
      response_msg += "411 ";
      response_msg += ":No recipient given (";
      response_msg += res.getCommand() + ")\r\n";
    } else if (param.size() < 2) { // ERR_NOTEXTTOSEND
      response_msg += "412 ";
      response_msg += ":No text to send\r\n";
    // } else if () { // ERR_CANNOTSENDTOCHAN
    // } else if () { // ERR_NOTOPLEVEL
    // } else if () { // ERR_WILDTOPLEVEL
    } else if (param.size() > 2) { // ERR_TOOMANYTARGETS
      response_msg += "407 ";
      for (int i = 0; i < param.size() - 1; ++i) {
        response_msg += param[i] + " ";
      }
      response_msg += ":ERR_TOOMANYTARGETS recipients. ";
      response_msg += param.back() + "\r\n";
    } else if (!targetCheck(serv_, param[0])) { // ERR_NOSUCHNICK
      response_msg += "401 ";
      response_msg += param[0] + " ";
      response_msg += ":No such nick/channel\r\n";
    // } else if () { // RPL_AWAY
    } else {
      response_msg += res.getCommand() + " ";
      response_msg += param[0] + " ";
      response_msg += param[1] + "\r\n";
    }
    return send_(serv_, client, response_msg, "perform: privMsg: ");
  }
  return true;
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
