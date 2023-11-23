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
          "001 " + client.getNick() + " " +
          ":Welcome to the Internet Relay Network " +
          client.getIdentify() + "\r\n";
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
    UMstring_Channel const &channel_map = serv.getChannelMap();
    return channel_map.find(target) != channel_map.end();
  } else {
    UMint_Client const &connection = serv.getConnection();
    return connection.find(target) != connection.end();
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

static bool channelCheck(Server &serv,
                         Vstring const &param,
                         std::string &msg) {
  UMstring_Channel const &channel_map = serv.getChannelMap();
  for (int i = 0; i < param.size(); ++i) {
    if (param[i][0] != '#') { continue; }
    if (channel_map.find(param[i]) == channel_map.end()) {
      msg += "403 ";
      msg += param[i] + " ";
      msg += ":No such channel\r\n";
      return false;
    }
  }
  return true;
}

static bool isClientJoined(Server &serv,
                           Client &client,
                           Vstring const &param,
                           std::string &msg) {
  UMstring_Channel const &channel_map = serv.getChannelMap();
  UMstring_bool const &joined_channel = client.getJoinedChannel();
  for (int i = 0; i < param.size(); ++i) {
    if (param[i][0] != '#') { continue; }
    if (!channel_map[param[i]].isJoined(client.getNick()) ||
        joined_channel.find(param[i]) == joined_channel.end()) {
      msg += "442 ";
      msg += param[i] + " ";
      msg += ":You're not on that channel\r\n";
      return false;
    }
  }
  return true;
}

static bool isChannelOper(Server &serv,
                          Client &client,
                          Vstring const &param,
                          std::string &msg) {
  UMstring_Channel const &channel_map = serv.getChannelMap();
  for (int i = 0; i < param.size(); ++i) {
    if (param[i][0] != '#') { continue; }
    if (!channel_map[param[i]].isOperator(client.getNick())) {
      msg += "482 ";
      msg += param[i] + " ";
      msg += ":You're not channel operator\r\n";
      return false;
    }
  }
  return true;
}

static void invite_(Server &serv,
                    Client &client,
                    Client &target,
                    Vstring const &param) {
  std::string msg;
  msg  = std::string(":") + client.getIdentify() + " ";
  msg += "INVITE ";
  msg += param[0] + " ";
  msg += param[1] + "\r\n";
  send_(serv, target, msg, "response: invite_: ");
}

static bool errNeedMoreParam(std::size_t mn,
                             std::string const &cmd,
                             Vstring const &param,
                             std::string &msg) {
  if (param.size() < mn) {
    msg += "461 ";
    msg += cmd + " ";
    msg += ":Not enough parameters\r\n";
    return false;
  }
  return true;
}

static void rplChannelModeIs(Vstring const &param, std::string &msg) {
  msg += "324 ";
  for (int i = 0; i < param.size(); ++i) {
    msg += param[i] + " ";
  }
  msg += "\r\n";
}

static Request makeRequest(int request_code,
                           int requester_sock,
                           int target_sock,
                           std::string const &command,
                           std::string const &addi,
                           Vstring const &param,
                           bool is_derived) {
  Request req;
  req.setRequestCode(request_code);
  req.setRequesterSocket(requester_sock);
  req.setTargetSocket(target_sock);
  req.setCommand(command);
  req.setAddi(addi);
  req.setParam(param);
  req.setDerived(is_derived);
  return req;
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
  callbacks_[Request::kAccept]  = accept;
  callbacks_[Request::kDeny]    = deny;
  callbacks_[Request::kTopic]   = topic;
  callbacks_[Request::kMode]    = mode;
}

void RequestCallback::operator()(Request const &req, RequestPool &requests) {
  int code = req.getRequestCode();
  if (code < 0 || code >= Request::kCount) {
    throw std::runtime_error(
        std::string("RequestCallback: invalid request code"));
  }
  UMint_Client &connection = serv_.getConnection();
  if (connection.find(req.getRequesterSocket()) != connection.end()) {
    callbacks_[code](req, requests);
  }
}

void RequestCallback::unknown(Request const &req, RequestPool &requests) {
  // ERR_UNKNOWNCOMMAND
  Client &client = serv_.getClient(req.getRequesterSocket());
  std::string msg = std::string(":") + serv_.getHost() + " ";
  msg += "421 ";
  msg += req.getCommand() + " ";
  msg += ":Unknown command\r\n";
  send_(serv_, client, msg, "response: unknown: ");
}

void RequestCallback::pass(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(1, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
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

void RequestCallback::nick(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  Vstring const &param = req.getParam();
  UMstring_int &nick_to_sock = serv_.getNickToSock();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (param.size() < 1) { // ERR_NONICKNAMEGIVEN
    msg += "431 ";
    msg += ":No nickname given\r\n";
  } else if (!nickCheck(param[0])) { // ERR_ERRONEUSNICKNAME
    msg += "432 ";
    msg += param[0] + " ";
    msg += ":Erroneous nickname\r\n";
  } else if (nick_to_sock.find(param[0]) != nick_to_sock.end()) {
    // ERR_NICKNAMEINUSE
    msg += "433 ";
    msg += param[0] + " ";
    msg += ":Nickname is already in use\r\n";
  } else {
    client.setNick(param[0]);
    nick_to_sock[param[0]] = client.getSocket();
    auth(serv_, client);
    return;
  }
  send_(serv_, client, msg, "response: nick: ");
}

void RequestCallback::user(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(1, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
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

void RequestCallback::privMsg(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client)) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (req.isDerived()) { // derived request
    msg  = std::string(":") + client.getIdentify() + " ";
    msg += req.getCommand() + " ";
    msg += serv_.getClient(req.getTargetSocket()).getNick() + " ";
    msg += param[1] + "\r\n";
    send_(serv_, serv_.getClient(req.getTargetSocket()), msg,
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
    msg += param[0] + " ";
    msg += ":ERR_TOOMANYTARGETS recipients. ";
    msg += param[1] + "\r\n";
  } else if (!targetCheck(serv_, param[0])) { // ERR_NOSUCHNICK
    msg += "401 ";
    msg += param[0] + " ";
    msg += ":No such nick/channel\r\n";
  } else { // make PRIVMSG
    if (param[0][0] == '#') { // to channel ...
      Channel &channel = serv_.getChannelMap()[param[0]];
      UMstring_bool &joined_client = channel.getJoinedClient();
      UMstring_bool::iterator i;
      for (i = joined_client.begin(); i != joined_client.end(); ++i) {
        if (client.getNick() == i->first) { continue; }
        requests.push(makeRequest(req.getRequestCode(),
                                  req.getRequesterSocket(),
                                  serv_.getNickToSock()[i->first],
                                  req.getCommand(),
                                  req.getAddi(),
                                  req.getParam(),
                                  true));
      }
    } else { // to user ...
      requests.push(makeRequest(req.getRequestCode(),
                                req.getRequesterSocket(),
                                req.getTargetSocket(),
                                req.getCommand(),
                                req.getAddi(),
                                req.getParam(),
                                true));
    }
    return;
  }
  send_(serv_, client, msg, "response: privMsg: ");
}

void RequestCallback::join(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client)) { return; }
  Vstring const &param = req.getParam();
  if (param[0] == "0") {
    // make PART
    Vstring param2;
    UMstring_bool &joined_channel = client.getJoinedChannel();
    UMstring_bool::iterator i;
    for (i = joined_channel.begin(); i != joined_channel.end(); ++i) {
      param2.push_back(i->first);
    }
    requests.push(makeRequest(Request::kPart,
                              req.getRequesterSocket(),
                              req.getTargetSocket(),
                              "PART",
                              req.getAddi(),
                              param2,
                              true));
    return;
  }
  Channel &channel = serv_.getChannelMap()[param[0]];
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (req.isDerived()) { // derived request
    msg  = std::string(":") + client.getIdentify() + " ";
    msg += req.getCommand() + " ";
    msg += param[0] + "\r\n";
    send_(serv_, serv_.getClient(req.getTargetSocket()), msg,
          "response: join: ");
    return;
  } else if (!errNeedMoreParam(1, req.getCommand(), param, msg)) {
    // ERR_NEEDMOREPARAM
  } else if (channel.getInviteOnly() &&
      !channel.isInvite(client.getNick())) { // ERR_INVITEONLYCHAN
    msg += "473 ";
    msg += param[0] + " ";
    msg += ":Cannot join channel (+i)\r\n";
  } else if (channel.getHasPassword() &&
      (param.size() < 2 || !channel.verify(param[1]))) { // ERR_BADCHANNELKEY
    msg += "475 ";
    msg += param[0] + " ";
    msg += ":Cannot join channel (+k)\r\n";
  } else if (channel.getHasLimit() &&
      channel.getJoined().size() >= channel.getLimit()) { // ERR_CHANNELISFULL
    msg += "471 ";
    msg += param[0] + " ";
    msg += ":Cannot join channel (+l)\r\n";
  } else if (client.getChannelMap().size() >= Client::kMaxChannel) {
                                                        // ERR_TOOMANYCHANNELS
    msg += param[0] + " ";
    msg += ":You have joined too many channels\r\n";
  } else {
    if (channel.getName() == "") {
      channel.setName(param[0]);
      channel.setOrigin(client.getNick());
    }
    channel.delInvite(client.getNick());
    channel.join(client.getNick());
    client.join(channel.getName());
    // make JOIN
    UMstring_bool &joined_client = channel.getJoinedClient();
    UMstring_bool::iterator i;
    for (i = joined_client.begin(); i != joined_client.end(); ++i) {
      requests.push(makeRequest(req.getRequestCode(),
                                req.getRequesterSocket(),
                                serv_.getNickToSock()[i->first],
                                req.getCommand(),
                                req.getAddi(),
                                req.getParam(),
                                true));
    }
    // make NAMES
    Vstring param2;
    param2.push_back(param[0]);
    requests.push(makeRequest(Request::kNames,
                              req.getRequesterSocket(),
                              req.getTargetSocket(),
                              "NAMES",
                              req.getAddi(),
                              param2,
                              false));
    return;
  }
  send_(serv_, client, msg, "response: join: ");
}

void RequestCallback::names(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client)) { return; }
  Vstring const &param = req.getParam();
  std::string msg;
  if (param.size() == 1 && param[0][0] == '#' && targetCheck(param[0])) {
    // RPL_NAMREPLY
    msg  = std::string(":") + serv_.getHost() + " ";
    msg += "353 ";
    msg += std::string("= ") + param[0] + " ";
    msg += ":";
    Channel &channel = serv_.getChannelMap()[param[0]];
    UMstring_Client &joined_client = channel.getJoinedClient();
    UMstring_Client::iterator i;
    for (i = joined_client.begin(); i != joined_client.end(); ++i) {
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

void RequestCallback::part(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client)) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(1, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!channelCheck(serv_, param, msg)) { // ERR_NOSUCHCHANNEL
  } else if (!isClientJoined(serv_, client, param, msg)) { // ERR_NOTONCHANNEL
  } else {
    UMstring_Channel &channel_map = serv.getChannelMap();
    for (int i = 0; i < param.size(); ++i) {
      Channel &channel = channel_map[param[i]];
      channel.part(client.getNick());
      client.part(param[i]);
      // send part message
      UMstring_bool joined_client = channel.getJoinedClient();
      UMstring_bool::iterator j;
      for (j = joined_client.begin(); j != joined_client.end(); ++j) {
        msg  = std::string(":") + client.getIdentify() + " ";
        msg += req.getCommand() + " ";
        msg += param[i] + " ";
        if (req.getAddi() == "") {
          msg += client.getNick() + " has left...\r\n";
        } else {
          msg += req.getAddi() + "\r\n";
        }
        send_(serv_, serv_.getClient(j->first), msg, "response: part: ");
      }
    }
    return;
  }
  send_(serv_, client, msg, "response: part: ");
}

void RequestCallback::kick(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client)) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(2, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!channelCheck(serv_, param, msg)) { // ERR_NOSUCHCHANNEL
  } else if (!isChannelOper(serv_, client, param, msg)) {// ERR_CHANOPRIVSNEEDED
  } else if (!serv_.getChannelMap()[param[0]].isJoined(param[1])) {
    // ERR_USERNOTINCHANNEL
  } else if (!isClientJoined(serv_, client, param, msg)) { // ERR_NOTONCHANNEL
  } else {
    Channel &channel = serv_.getChannelMap()[param[0]];
    Client &kicked = serv_.getClient(param[1]);
    channel.part(param[1]);
    kicked.part(param[0]);
    if (req.getAddi() != "") {
      msg  = std::string(":") + client.getIdentify() + " ";
      msg += req.getCommand() + " ";
      msg += param[1] + " ";
      msg += param[0] + " ";
      msg += req.getAddi() + "\r\n";
      UMstring_bool joined_client = channel.getJoinedClient();
      UMstring_bool::iterator i;
      for (i = joined_client.begin(); i != joined_client.end(); ++i) {
        send_(serv_, serv_.getClient(i->first), msg, "response: kick: ");
      }
    }
    return;
  }
  send_(serv_, client, msg, "response: kick: ");
}

void RequestCallback::invite(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client)) { return; }
  Vstring const &param = req.getParam();
  Channel &channel = serv_.getChannelMap()[param[1]];
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(2, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!targetCheck(serv_, param[0])) { // ERR_NOSUCHNICK
    msg += "401 ";
    msg += param[0] + " ";
    msg += ":No such nick/channel\r\n";
  } else if (!isClientJoined(serv_, client, param, msg)) { // ERR_NOTONCHANNEL
  } else if (channel.isJoined(param[0]) &&
      client.isJoined(param[1])) { // ERR_USERONCHANNEL
    msg += "443 ";
    msg += param[0] + " ";
    msg += param[1] + " ";
    msg += ":is already on channel\r\n";
  } else if (channel.getInviteOnly() &&
      !isChannelOper(serv_, client, param, msg)) { // ERR_CHANOPRIVSNEEDED
  } else { // RPL_INVITING
    msg += "341 ";
    msg += param[1] + " ";
    msg += param[0] + "\r\n";
    channel.addInvite(param[0]);
    invite_(serv_, client, serv_.getClient(param[0]), param);
  }
  send_(serv_, client, msg, "response: invite: ");
}

void RequestCallback::accept(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client)) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(1, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!channelCheck(serv_, param, msg)) { // ERR_NOSUCHCHANNEL
  } else {
    Channel &channel = serv_.getChannelMap()[param[0]];
    if (!channel.isInvite(client.getNick())) { return; }
    Vstring param2;
    param2.push_back(param[0]);
    requests.push(makeRequest(Request::kJoin,
                              req.getRequesterSocket(),
                              req.getTargetSocket(),
                              "JOIN",
                              req.getAddi(),
                              param2,
                              true));
    return;
  }
  send_(serv_, client, msg, "response: accept: ");
}

void RequestCallback::deny(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client)) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(1, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!channelCheck(serv_, param, msg)) { // ERR_NOSUCHCHANNEL
  } else {
    Channel &channel = serv_.getChannelMap()[param[0]];
    channel.delInvite(client.getNick());
    return;
  }
  send_(serv_, client, msg, "response: deny: ");
}

void RequestCallback::topic(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client)) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(1, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!isClientJoined(serv_, client, param, msg)) { // ERR_NOTONCHANNEL
  } else if (channel.getOperTopic() &&
      !isChannelOper(serv_, client, param, msg)) { // ERR_CHANOPRIVSNEEDED
  } else {
    Channel &channel = serv_.getChannelMap()[param[0]];
    if (channel.getTopic() == "") { // RPL_NOTOPIC
      msg += "331 ";
      msg += param[0] + " ";
      msg += ":No topic is set\r\n";
    } else { // RPL_TOPIC  
      msg += "332 ";
      msg += param[0] + " ";
      msg += std::string(":") + channel.getTopic() + "\r\n";
    }
  }
  send_(serv_, client, msg, "response: topic: ");
}

void RequestCallback::mode(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client)) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(2, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!channelCheck(serv_, param, msg)) { // ERR_NOSUCHCHANNEL
  } else if (!isChannelOper(serv_, client, param, msg)) {// ERR_CHANOPRIVSNEEDED
  } else if (param[1].size() != 2 ||
      (param[1][0] != '-' && param[1][0] != '+') ||
      std::string("oitkl").find(param[1][1]) == std::string::npos) {
    // ERR_UNKNOWNMODE
    msg += "472 ";
    msg += param[1] + " ";
    msg += ":is unknown mode char to me for ";
    msg += param[0] + "\r\n";
  } else {
    Channel &channel = serv_.getChannelMap()[param[0]];
    if (param[1][1] == 'o') {
      if (!errNeedMoreParam(3, req.getCommand(), param, msg)) {
        // ERR_NEEDMOREPARAM
      } else if (serv_.getChannelMap()[param[0]].isJoined(param[2])) {
        // ERR_USERNOTINCHANNEL
      } else { // RPL_CHANNELMODEIS
        rplChannelModeIs(param, msg);
        if (param[1][0] == '+') {
          channel.addOperator(param[2]);
        } else {
          channel.delOperator(param[2]);
        }
      }
    } else if (param[1][1] == 'i') {
      if (!errNeedMoreParam(2, req.getCommand(), param, msg)) {
        // ERR_NEEDMOREPARAM
      } else { // RPL_CHANNELMODEIS
        rplChannelModeIs(param, msg);
        channel.setInviteOnly(param[1][0] == '+');
      }
    } else if (param[1][1] == 't') {
      if (!errNeedMoreParam(2, req.getCommand(), param, msg)) {
        // ERR_NEEDMOREPARAM
      } else { // RPL_CHANNELMODEIS
        rplChannelModeIs(param, msg);
        channel.setOperTopic(param[1][0] == '+');
      }
    } else if (param[1][1] == 'k') {
      if (!errNeedMoreParam(
            param[1][0] == '+' ? 3 : 2, req.getCommand(), param, msg)) {
        // ERR_NEEDMOREPARAM
      } else { // RPL_CHANNELMODEIS
        rplChannelModeIs(param, msg);
        channel.setHasPassword(param[1][0] == '+');
        if (param[1][0] == '+') {
          channel.setPassword(param[2]);
        }
      }
    } else {
      if (!errNeedMoreParam(
            param[1][0] == '+' ? 3 : 2, req.getCommand(), param, msg)) {
        // ERR_NEEDMOREPARAM
      } else { // RPL_CHANNELMODEIS
        rplChannelModeIs(param, msg);
        channel.setHasLimit(param[1][0] == '+');
        if (param[1][0] == '+') {
          channel.setLimit(string_to<std::size_t>(param[2]));
        }
      }
    }
  }
  send_(serv_, client, msg, "response: mode: ");
}

} // namespace irc
