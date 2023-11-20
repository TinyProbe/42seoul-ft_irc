#include "RequestCallback.h"

namespace irc {

static void send_(Server &serv,
                  Client const &client,
                  std::string const &msg,
                  std::string const &call_stack) {
  if (!client.canWrite()) {
    throw std::runtime_error("can't write");
  }
  if (send(client.getSocket(),
           msg.c_str(),
           msg.size(),
           MSG_DONTWAIT | MSG_NOSIGNAL) == -1) {
    if (errno == EAGAIN) {
      throw std::runtime_error("blocked send");
    } else {
      std::cerr << call_stack + std::string(strerror(errno)) << std::endl;
      serv.disconnect(client.getSocket());
    }
  }
}

static void auth(Server const &serv, Client &client) {
  if (!client.getAuth() && client.getNick() != "*" && client.getUser() != "*") {
    if (serv_.verify(client.getPassword())) { // RPL_WELCOME
      std::string msg =
          std::string(":") + serv.getHost() + " " +
          "001" + client.getNick() + " " +
          ":Welcome to the Internet Relay Network " +
          client.getNick() + "!" + client.getUser() + "@" +
          client.getHost() + "\r\n";
      send_(serv, client, msg, "perform: auth: ");
      client.setAuth(true);
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

static bool verify(Server &serv, Client const &client) {
  if (!client.getAuth()) { // ERR_NOTREGISTERED
    std::string msg = std::string(":") + serv.getHost() + " ";
    msg += "451 ";
    msg += ":You have not registered\r\n";
    send_(serv, client, msg, "response: verify: ");
    return false;
  }
  return true;
}

RequestCallback::RequestCallback(Server &serv)
    : serv_(serv) {
  callbacks_.resize(Request::kCount);
  callbacks_[Request::kUnknown] = unknown;
  callbacks_[Request::kPass]    = pass;
  callbacks_[Request::kNick]    = nick;
  callbacks_[Request::kUser]    = user;
  callbacks_[Request::kPrivMsg] = privMsg;
  callbacks_[Request::kJoin]    = join;
  callbacks_[Request::kNames]   = names;
  callbacks_[Request::kPart]    = part;
  callbacks_[Request::kKick]    = kick;
  callbacks_[Request::kInvite]  = invite;
  callbacks_[Request::kTopic]   = topic;
  callbacks_[Request::kMode]    = mode;
}

void RequestCallback::operator()(Request const &req, RequestPool &requests) {
  int code = req.getRequestCode();
  if (code < 0 || code >= Request::kCount) {
    throw std::runtime_error(
        std::string("RequestCallback: invalid request code"));
  }
  return callbacks_[code](req, requests);
}

void RequestCallback::unknown(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) { // ERR_UNKNOWNCOMMAND
    Client &client = it->second;
    std::string msg = std::string(":") + serv_.getHost() + " ";
    msg += "421 ";
    msg += req.getCommand() + " ";
    msg += ":Unknown command" + "\r\n";
    send_(serv_, client, msg, "response: unknown: ");
  }
}

void RequestCallback::pass(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    std::vector<std::string> const &param = req.getParam();
    std::string msg = std::string(":") + serv_.getHost() + " ";
    if (param.size() < 1) { // ERR_NEEDMOREPARAM
      msg += "461 ";
      msg += req.getCommand() + " ";
      msg += ":Not enough parameters\r\n";
    } else if (client.getAuth()) { // ERR_ALREADYREGISTERED
      msg += "462 ";
      msg += ":Unauthorized command (already registered)\r\n";
    } else {
      client.setPassword(param[0]);
      auth(serv_, client);
      return;
    }
    send_(serv_, client, msg, "response: pass: ");
  }
}

void RequestCallback::nick(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    std::vector<std::string> const &param = req.getParam();
    std::string msg = std::string(":") + serv_.getHost() + " ";
    if (param.size() < 1) { // ERR_NONICKNAMEGIVEN
      msg += "431 ";
      msg += ":No nickname given\r\n";
    } else if (!nickCheck(param[0])) { // ERR_ERRONEUSNICKNAME
      msg += "432 ";
      msg += param[0] + " ";
      msg += ":Erroneous nickname\r\n";
    } else if (serv_.getNickToSock().find(param[0]) !=
               serv_.getNickToSock().end()) { // ERR_NICKNAMEINUSE
      msg += "433 ";
      msg += param[0] + " ";
      msg += ":Nickname is already in use\r\n";
    } else {
      client.setNick(param[0]);
      auth(serv_, client);
      return;
    }
    send_(serv_, client, msg, "response: nick: ");
  }
}

void RequestCallback::user(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    std::vector<std::string> const &param = req.getParam();
    std::string msg = std::string(":") + serv_.getHost() + " ";
    if (param.size() < 1) { // ERR_NEEDMOREPARAM
      msg += "461 ";
      msg += req.getCommand() + " ";
      msg += ":Not enough parameters\r\n";
    } else if (client.getAuth()) { // ERR_ALREADYREGISTERED
      msg += "462 ";
      msg += ":Unauthorized command (already registered)\r\n";
    } else {
      client.setUser(param[0]);
      auth(serv_, client);
      return;
    }
    send_(serv_, client, msg, "response: user: ");
  }
}

void RequestCallback::privMsg(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    if (!verify(serv_, client)) { return; }
    std::vector<std::string> const &param = req.getParam();
    std::string msg = std::string(":") + serv_.getHost() + " ";
    if (req.isDerived()) { // derived request
      msg  = std::string(":") + client.getIdentify() + " ";
      msg += res.getCommand() + " ";
      msg += param[0] + " ";
      msg += param[1] + "\r\n";
      send_(serv_, connection[req.getTargetSocket()], msg,
            "response: privMsg: ");
      return;
    } else if (param.size() < 1) { // ERR_NORECIPIENT
      msg += "411 ";
      msg += ":No recipient given (";
      msg += req.getCommand() + ")\r\n";
    } else if (param.size() < 2) { // ERR_NOTEXTTOSEND
      msg += "412 ";
      msg += ":No text to send\r\n";
    } else if (param.size() > 2) { // ERR_TOOMANYTARGETS
      msg += "407 ";
      for (int i = 0; i < param.size() - 1; ++i) {
        msg += param[i] + " ";
      }
      msg += ":ERR_TOOMANYTARGETS recipients. ";
      msg += param.back() + "\r\n";
    } else if (!targetCheck(serv_, param[0])) { // ERR_NOSUCHNICK
      msg += "401 ";
      msg += param[0] + " ";
      msg += ":No such nick/channel\r\n";
    } else {
      // ... 
      return;
    }
    send_(serv_, client, msg, "response: privMsg: ");
  }
}

void RequestCallback::join(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    if (!verify(serv_, client)) { return; }
    std::vector<std::string> const &param = req.getParam();
    std::string msg = std::string(":") + serv_.getHost() + " ";
    Channel const &channel = serv_.getChannelMap()[param[0]];
    if (req.isDerived()) { // derived request
      msg  = std::string(":") + client.getIdentify() + " ";
      msg += res.getCommand() + " ";
      msg += param[0] + "\r\n";
      send_(serv_, connection[req.getTargetSocket()], msg, "response: join: ");
      return;
    } else if (param.size() < 1) { // ERR_NEEDMOREPARAMS
      msg += "461 ";
      msg += req.getCommand() + " ";
      msg += ":Not enough parameters\r\n";
    } else if (channel.isBanned(client.getNick())) { // ERR_BANNEDFROMCHAN
      msg += "474 ";
      msg += param[0] + " ";
      msg += ":Cannot join channel (+b)\r\n";
    } else if (channel.getInviteOnly()) { // ERR_INVITEONLYCHAN
      msg += "473 ";
      msg += param[0] + " ";
      msg += ":Cannot join channel (+i)\r\n";
    } else if (channel.getHasPassword() &&
        (param.size() < 2 || !channel.verify(param[1]))) { // ERR_BADCHANNELKEY
      msg += "475 ";
      msg += param[0] + " ";
      msg += ":Cannot join channel (+k)\r\n";
    } else if (channel.getConnection().size() >= channel.getLimit()) {
                                                        // ERR_CHANNELISFULL
      msg += "471 ";
      msg += param[0] + " ";
      msg += ":Cannot join channel (+l)\r\n";
    } else if (client.getChannelMap().size() >= Client::kMaxChannel) {
                                                        // ERR_TOOMANYCHANNELS
      msg += param[0] + " ";
      msg += ":You have joined too many channels\r\n";
    } else {
      // ...
      return;
    }
    send_(serv_, client, msg, "response: join: ");
  }
}

void RequestCallback::names(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    if (!verify(serv_, client)) { return; }
    std::vector<std::string> const &param = req.getParam();
    std::string msg;
    if (param.size() == 1 && param[0][0] == "#" && targetCheck(param[0])) {
      // RPL_NAMREPLY
      msg  = std::string(":") + serv_.getHost() + " ";
      msg += "353 ";
      msg += std::string("= ") + param[0] + " ";
      msg += ":";
      Channel const &channel = serv_.getChannelMap()[param[0]];
      Connection const &connection = channel.getConnection();
      Connection::iterator i;
      for (i = connection.begin(); i != connection.end(); ++i) {
        std::string const &nick = i->second.getNick();
        if (channel.isOperator(nick) == 1) { msg += "@"; }
        msg += nick + " ";
      }
      msg += "\r\n";
      send_(serv_, client, msg, "response: names: ");
    }
    // RPL_ENDOFNAMES
    msg  = std::string(":") + serv_.getHost() + " ";
    msg += "366 ";
    msg += param[0] + " ";
    msg += ":End of NAMES list\r\n";
    send_(serv_, client, msg, "response: names: ");
  }
}

void RequestCallback::part(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    if (!verify(serv_, client)) { return; }
    std::vector<std::string> const &param = req.getParam();
    std::string msg = std::string(":") + serv_.getHost() + " ";
    Channel const &channel = serv_.getChannelMap()[param[0]];
    if (req.isDerived()) { // derived request
      channel.
    } else if () { // ERR_NEEDMOREPARAMS
    } else if () { // ERR_NOSUCHCHANNEL
    } else if () { // ERR_NOTONCHANNEL
    } else {
    }
    send_(serv_, client, msg, "response: part: ");
  }
}

void RequestCallback::kick(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    if (!verify(serv_, client)) { return; }
    std::vector<std::string> const &param = req.getParam();
    std::string msg = std::string(":") + serv_.getHost() + " ";

    send_(serv_, client, msg, "response: kick: ");
  }
}

void RequestCallback::invite(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    if (!verify(serv_, client)) { return; }
    std::vector<std::string> const &param = req.getParam();
    std::string msg = std::string(":") + serv_.getHost() + " ";

    send_(serv_, client, msg, "response: invite: ");
  }
}

void RequestCallback::topic(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    if (!verify(serv_, client)) { return; }
    std::vector<std::string> const &param = req.getParam();
    std::string msg = std::string(":") + serv_.getHost() + " ";

    send_(serv_, client, msg, "response: topic: ");
  }
}

void RequestCallback::mode(Request const &req, RequestPool &requests) {
  Connection &connection = serv_.getConnection();
  Connection::iterator it = connection.find(req.getRequesterSocket());
  if (it != connection.end()) {
    Client &client = it->second;
    if (!verify(serv_, client)) { return; }
    std::vector<std::string> const &param = req.getParam();
    std::string msg = std::string(":") + serv_.getHost() + " ";

    send_(serv_, client, msg, "response: mode: ");
  }
}

} // namespace irc
