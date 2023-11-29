/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestCallback.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkong <tkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 22:17:02 by tkong             #+#    #+#             */
/*   Updated: 2023/11/29 22:17:02 by tkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestCallback.h"

#include <iostream>

#include "common.h"
#include "Server.h"
#include "Client.h"
#include "RequestPool.h"

namespace irc {

static void send_(Server &serv,
                  Client const &client,
                  std::string const &msg,
                  std::string const &call_stack) {
  if (!client.canWrite()) {
    throw std::runtime_error("can't write");
  }

#ifdef _DEBUG_
  std::cout << msg << std::endl;
#endif

  if (send(client.getSocket(),
           msg.c_str(),
           msg.size(),
           MSG_DONTWAIT/*  | MSG_NOSIGNAL */) == -1) {
    if (errno == EAGAIN) {
      throw std::runtime_error("blocked send");
    } else {
      std::cerr << call_stack + std::string(strerror(errno)) << std::endl;
      serv.disconnect(client.getSocket());
    }
  }
}

static void auth(Server &serv, Client &client) {
  if (!client.getAuth() && client.getNick() != "*" && client.getUser() != "*") {
    if (serv.verify(client.getPassword())) { // RPL_WELCOME
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
  for (int i = 1; i < (int)nick.size(); ++i) {
    if (!isalpha(nick[i]) && !isdigit(nick[i]) &&
        std::string("[]\\`_^{|}-").find(nick[i]) == std::string::npos) {
      return false;
    }
  }
  return true;
}

static bool targetCheck(Server &serv, std::string const &target) {
  if (target[0] == '#') {
    UMstring_Channel &channel_map = serv.getChannelMap();
    return channel_map.find(target) != channel_map.end();
  } else {
    UMstring_int &nick_to_sock = serv.getNickToSock();
    return nick_to_sock.find(target) != nick_to_sock.end();
  }
}

static bool verify(Server &serv, Client const &client, std::string const &cmd) {
  if (!client.getAuth()) { // ERR_NOTREGISTERED
    std::string msg = std::string(":") + serv.getHost() + " ";
    msg += "451 ";
    msg += cmd + " ";
    msg += ":You have not registered\r\n";
    send_(serv, client, msg, "response: verify: ");
    return false;
  }
  return true;
}

static bool channelCheck(Server &serv,
                         std::string const &channel,
                         std::string &msg) {
  UMstring_Channel &channel_map = serv.getChannelMap();
  if (channel_map.find(channel) == channel_map.end()) {
    msg += "403 ";
    msg += channel + " ";
    msg += ":No such channel\r\n";
    return false;
  }
  return true;
}

static bool isClientJoined(Server &serv,
                           Client &client,
                           std::string const &channel,
                           std::string &msg) {
  UMstring_Channel &channel_map = serv.getChannelMap();
  UMstring_bool &joined_channel = client.getJoinedChannel();
  UMstring_Channel::iterator it = channel_map.find(channel);
  if (it == channel_map.end() ||
      !it->second.isJoined(client.getNick()) ||
      joined_channel.find(channel) == joined_channel.end()) {
    msg += "442 ";
    msg += channel + " ";
    msg += ":You're not on that channel\r\n";
    return false;
  }
  return true;
}

static bool isChannelOper(Server &serv,
                          Client &client,
                          std::string const &channel,
                          std::string &msg) {
  UMstring_Channel &channel_map = serv.getChannelMap();
  UMstring_Channel::const_iterator it = channel_map.find(channel);
  if (it == channel_map.end() ||
      !it->second.isOperator(client.getNick())) {
    msg += "482 ";
    msg += channel + " ";
    msg += ":You're not channel operator\r\n";
    return false;
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

static bool errNeedMoreParam(size_t mn,
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
  for (int i = 0; i < (int)param.size(); ++i) {
    msg += param[i] + " ";
  }
  msg += "\r\n";
}

static Request makeRequest(int request_code,
                           int requester_sock,
                           std::string const &target,
                           std::string const &command,
                           std::string const &addi,
                           Vstring const &param,
                           bool is_derived) {
  Request req;
  req.setRequestCode(request_code);
  req.setRequesterSocket(requester_sock);
  req.setTarget(target);
  req.setCommand(command);
  req.setAddi(addi);
  req.setParam(param);
  req.setDerived(is_derived);
  return req;
}

RequestCallback::RequestCallback(Server &serv)
    : serv_(serv) {
}

void RequestCallback::operator()(Request const &req, RequestPool &requests) {
  int code = req.getRequestCode();
  if (code < 0 || code >= Request::kCount) {
    throw std::runtime_error(
        std::string("RequestCallback: invalid request code"));
  }
  UMint_Client &connection = serv_.getConnection();
  if (connection.find(req.getRequesterSocket()) != connection.end()) {
#ifdef _DEBUG_
    std::cout << "exist client: ";
#endif
    if (code == Request::kUnknown) {
#ifdef _DEBUG_
      std::cout << "unknown()" << std::endl;
#endif
      unknown(req, requests);
    } else if (code == Request::kPass) {
#ifdef _DEBUG_
      std::cout << "pass()" << std::endl;
#endif
      pass(req, requests);
    } else if (code == Request::kNick) {
#ifdef _DEBUG_
      std::cout << "nick()" << std::endl;
#endif
      nick(req, requests);
    } else if (code == Request::kUser) {
#ifdef _DEBUG_
      std::cout << "user()" << std::endl;
#endif
      user(req, requests);
    } else if (code == Request::kPrivMsg) {
#ifdef _DEBUG_
      std::cout << "privmsg()" << std::endl;
#endif
      privMsg(req, requests);
    } else if (code == Request::kJoin) {
#ifdef _DEBUG_
      std::cout << "join()" << std::endl;
#endif
      join(req, requests);
    } else if (code == Request::kNames) {
#ifdef _DEBUG_
      std::cout << "names()" << std::endl;
#endif
      names(req, requests);
    } else if (code == Request::kPart) {
#ifdef _DEBUG_
      std::cout << "part()" << std::endl;
#endif
      part(req, requests);
    } else if (code == Request::kKick) {
#ifdef _DEBUG_
      std::cout << "kick()" << std::endl;
#endif
      kick(req, requests);
    } else if (code == Request::kInvite) {
#ifdef _DEBUG_
      std::cout << "invite()" << std::endl;
#endif
      invite(req, requests);
    } else if (code == Request::kAccept) {
#ifdef _DEBUG_
      std::cout << "accept()" << std::endl;
#endif
      accept(req, requests);
    } else if (code == Request::kDeny) {
#ifdef _DEBUG_
      std::cout << "deny()" << std::endl;
#endif
      deny(req, requests);
    } else if (code == Request::kTopic) {
#ifdef _DEBUG_
      std::cout << "topic()" << std::endl;
#endif
      topic(req, requests);
    } else if (code == Request::kMode) {
#ifdef _DEBUG_
      std::cout << "mode()" << std::endl;
#endif
      mode(req, requests);
    } else if (code == Request::kPong) {
#ifdef _DEBUG_
      std::cout << "just ignore pong" << std::endl;
#endif
    } else if (code == Request::kQuit) {
#ifdef _DEBUG_
      std::cout << "quit()" << std::endl;
#endif
      quit(req, requests);
    }
  } else {
#ifdef _DEBUG_
    std::cout << "doesn't exist client: " <<
                 req.getRequesterSocket() << std::endl;
#endif
  }
}

void RequestCallback::unknown(Request const &req, RequestPool &requests) {
  (void) requests;
  // ERR_UNKNOWNCOMMAND
  Client &client = serv_.getClient(req.getRequesterSocket());
  std::string msg = std::string(":") + serv_.getHost() + " ";
  msg += "421 ";
  msg += req.getCommand() + " ";
  msg += ":Unknown command\r\n";
  send_(serv_, client, msg, "response: unknown: ");
}

void RequestCallback::pass(Request const &req, RequestPool &requests) {
  (void) requests;
  Client &client = serv_.getClient(req.getRequesterSocket());
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(1, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (client.getAuth()) { // ERR_ALREADYREGISTERED
    msg += "462 ";
    msg += req.getCommand() + " ";
    msg += ":Unauthorized command (already registered)\r\n";
  } else {
    client.setPassword(param[0]);
    auth(serv_, client);
    return;
  }
  send_(serv_, client, msg, "response: pass: ");
}

void RequestCallback::nick(Request const &req, RequestPool &requests) {
  (void) requests;
  Client &client = serv_.getClient(req.getRequesterSocket());
  Vstring const &param = req.getParam();
  UMstring_int &nick_to_sock = serv_.getNickToSock();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (param.size() < 1) { // ERR_NONICKNAMEGIVEN
    msg += "431 ";
    msg += req.getCommand() + " ";
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
    msg  = std::string(":") + client.getIdentify() + " ";
    msg += req.getCommand() + " ";
    msg += param[0] + "\r\n";

    // syncronize with channel
    UMstring_Channel &channel_map = serv_.getChannelMap();
    UMstring_bool &joined_channel = client.getJoinedChannel();
    UMstring_bool::iterator i;
    for (i = joined_channel.begin(); i != joined_channel.end(); ++i) {
      Channel &channel = channel_map[i->first];
      channel.part(client.getNick());
      channel.join(param[0]);
      int is_oper = channel.isOperator(client.getNick());
      if (is_oper == 1) {
        channel.setOrigin(param[0]);
      } else if (is_oper == 2) {
        channel.delOperator(client.getNick());
        channel.addOperator(param[0]);
      }
    }
    nick_to_sock.erase(client.getNick());
    nick_to_sock[param[0]] = client.getSocket();
    client.setNick(param[0]);

    auth(serv_, client);
  }
  send_(serv_, client, msg, "response: nick: ");
}

void RequestCallback::user(Request const &req, RequestPool &requests) {
  (void) requests;
  Client &client = serv_.getClient(req.getRequesterSocket());
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(4, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (client.getAuth()) { // ERR_ALREADYREGISTERED
    msg += "462 ";
    msg += req.getCommand() + " ";
    msg += ":Unauthorized command (already registered)\r\n";
  } else {
    client.setUser(param[0]);
    client.setReal(param[3]);
    auth(serv_, client);
    return;
  }
  send_(serv_, client, msg, "response: user: ");
}

void RequestCallback::privMsg(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client, req.getCommand())) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (req.isDerived()) { // derived request
    msg  = std::string(":") + client.getIdentify() + " ";
    msg += req.getCommand() + " ";
    msg += req.getTarget() + " ";
    msg += param[1] + "\r\n";
    send_(serv_, serv_.getClient(req.getAddi()), msg,
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
      UMstring_Channel &channel_map = serv_.getChannelMap();
      Channel &channel = channel_map[param[0]];
      UMstring_bool &joined_client = channel.getJoinedClient();
      UMstring_bool::iterator i;
      for (i = joined_client.begin(); i != joined_client.end(); ++i) {
        if (client.getNick() == i->first) { continue; }
        requests.push(makeRequest(req.getRequestCode(),
                                  req.getRequesterSocket(),
                                  req.getTarget(),
                                  req.getCommand(),
                                  i->first,
                                  req.getParam(),
                                  true));
      }
    } else { // to user ...
      if (client.getNick() == req.getTarget()) { return; }
      requests.push(makeRequest(req.getRequestCode(),
                                req.getRequesterSocket(),
                                req.getTarget(),
                                req.getCommand(),
                                req.getTarget(),
                                req.getParam(),
                                true));
    }
    return;
  }
  send_(serv_, client, msg, "response: privMsg: ");
}

void RequestCallback::join(Request const &req, RequestPool &requests) {
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client, req.getCommand())) { return; }
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
                              req.getTarget(),
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
    send_(serv_, serv_.getClient(req.getTarget()), msg, "response: join: ");
    return;
  } else if (!errNeedMoreParam(1, req.getCommand(), param, msg)) {
    // ERR_NEEDMOREPARAM
  } else if (channel.getInviteOnly() &&
      !channel.isInvited(client.getNick())) { // ERR_INVITEONLYCHAN
    msg += "473 ";
    msg += param[0] + " ";
    msg += ":Cannot join channel (+i)\r\n";
  } else if (channel.getHasPassword() &&
      (param.size() < 2 || !channel.verify(param[1])) &&
      !channel.isInvited(client.getNick())) { // ERR_BADCHANNELKEY
    msg += "475 ";
    msg += param[0] + " ";
    msg += ":Cannot join channel (+k)\r\n";
  } else if (channel.getHasLimit() &&
      channel.getJoinedClient().size() >= channel.getLimit()) {
    // ERR_CHANNELISFULL
    msg += "471 ";
    msg += param[0] + " ";
    msg += ":Cannot join channel (+l)\r\n";
  } else if ((int)client.getJoinedChannel().size() >= Client::kMaxChannel) {
    // ERR_TOOMANYCHANNELS
    msg += "405 ";
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
                                i->first,
                                req.getCommand(),
                                req.getAddi(),
                                req.getParam(),
                                true));
    }
    // make TOPIC
    requests.push(makeRequest(Request::kTopic,
                              req.getRequesterSocket(),
                              req.getTarget(),
                              "TOPIC",
                              req.getAddi(),
                              req.getParam(),
                              false));
    // make NAMES
    Vstring param2;
    param2.push_back(param[0]);
    requests.push(makeRequest(Request::kNames,
                              req.getRequesterSocket(),
                              req.getTarget(),
                              "NAMES",
                              req.getAddi(),
                              param2,
                              false));
    return;
  }
  if (channel.getJoinedClient().empty()) {
    serv_.getChannelMap().erase(param[0]);
  }
  send_(serv_, client, msg, "response: join: ");
}

void RequestCallback::names(Request const &req, RequestPool &requests) {
  (void) requests;
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client, req.getCommand())) { return; }
  Vstring const &param = req.getParam();
  if (param.size() != 1) { return; }
  std::string msg;
  if (param[0][0] == '#' && targetCheck(serv_, param[0])) {
    // RPL_NAMREPLY
    msg  = std::string(":") + serv_.getHost() + " ";
    msg += "353 ";
    msg += std::string("= ") + param[0] + " ";
    msg += ":";
    Channel &channel = serv_.getChannelMap()[param[0]];
    UMstring_bool &joined_client = channel.getJoinedClient();
    UMstring_bool::iterator i;
    for (i = joined_client.begin(); i != joined_client.end(); ++i) {
      std::string const &nick = i->first;
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
  (void) requests;
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client, req.getCommand())) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(1, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!channelCheck(serv_, param[0], msg)) { // ERR_NOSUCHCHANNEL
  } else if (!isClientJoined(serv_, client, param[0], msg)) { // ERR_NOTONCHANNEL
  } else {
    // send part message
    msg  = std::string(":") + client.getIdentify() + " ";
    msg += req.getCommand() + " ";
    msg += param[0] + " ";
    if (req.getAddi() == "") {
      msg += client.getNick() + "\r\n";
    } else {
      msg += req.getAddi() + "\r\n";
    }
    UMstring_Channel &channel_map = serv_.getChannelMap();
    Channel &channel = channel_map[param[0]];
    UMstring_bool joined_client = channel.getJoinedClient();
    UMstring_bool::iterator i;
    for (i = joined_client.begin(); i != joined_client.end(); ++i) {
      send_(serv_, serv_.getClient(i->first), msg, "response: part: ");
    }
    channel.part(client.getNick());
    client.part(param[0]);
    if (joined_client.empty()) { channel_map.erase(param[0]); }
    return;
  }
  send_(serv_, client, msg, "response: part: ");
}

void RequestCallback::kick(Request const &req, RequestPool &requests) {
  (void) requests;
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client, req.getCommand())) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(2, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!channelCheck(serv_, param[0], msg)) { // ERR_NOSUCHCHANNEL
  } else if (!isChannelOper(serv_, client, param[0], msg)) {// ERR_CHANOPRIVSNEEDED
  } else if (!serv_.getChannelMap()[param[0]].isJoined(param[1])) {
    // ERR_USERNOTINCHANNEL
    msg += "441 ";
    msg += client.getNick() + " ";
    msg += param[0] + " ";
    msg += ":They aren't on that channel\r\n";
  } else if (!isClientJoined(serv_, client, param[0], msg)) { // ERR_NOTONCHANNEL
  } else {
    // make part
    Vstring param2;
    param2.push_back(param[0]);
    requests.push(makeRequest(Request::kPart,
                              serv_.getClient(param[1]).getSocket(),
                              "",
                              "PART",
                              "",
                              param2,
                              false));
    Channel &channel = serv_.getChannelMap()[param[0]];
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
  (void) requests;
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client, req.getCommand())) { return; }
  Vstring const &param = req.getParam();
  Channel &channel = serv_.getChannelMap()[param[1]];
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(2, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!targetCheck(serv_, param[0])) { // ERR_NOSUCHNICK
    msg += "401 ";
    msg += param[0] + " ";
    msg += ":No such nick/channel\r\n";
  } else if (!isClientJoined(serv_, client, param[1], msg)) {// ERR_NOTONCHANNEL
  } else if (channel.isJoined(param[0]) && client.isJoined(param[1])) {
    // ERR_USERONCHANNEL
    msg += "443 ";
    msg += param[0] + " ";
    msg += param[1] + " ";
    msg += ":is already on channel\r\n";
  } else if (channel.getInviteOnly() &&
      !isChannelOper(serv_, client, param[1], msg)) { // ERR_CHANOPRIVSNEEDED
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
  if (!verify(serv_, client, req.getCommand())) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(1, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!channelCheck(serv_, param[0], msg)) { // ERR_NOSUCHCHANNEL
  } else {
    Channel &channel = serv_.getChannelMap()[param[0]];
    if (!channel.isInvited(client.getNick())) { return; }
    Vstring param2;
    param2.push_back(param[0]);
    requests.push(makeRequest(Request::kJoin,
                              req.getRequesterSocket(),
                              req.getTarget(),
                              "JOIN",
                              req.getAddi(),
                              param2,
                              false));
    return;
  }
  send_(serv_, client, msg, "response: accept: ");
}

void RequestCallback::deny(Request const &req, RequestPool &requests) {
  (void) requests;
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client, req.getCommand())) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(1, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!channelCheck(serv_, param[0], msg)) { // ERR_NOSUCHCHANNEL
  } else {
    Channel &channel = serv_.getChannelMap()[param[0]];
    channel.delInvite(client.getNick());
    return;
  }
  send_(serv_, client, msg, "response: deny: ");
}

void RequestCallback::topic(Request const &req, RequestPool &requests) {
  (void) requests;
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client, req.getCommand())) { return; }
  Vstring const &param = req.getParam();
  UMstring_Channel &channel_map = serv_.getChannelMap();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(1, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!isClientJoined(serv_, client, param[0], msg)) {// ERR_NOTONCHANNEL
  } else if (channel_map[param[0]].getOperTopic() &&
      !isChannelOper(serv_, client, param[0], msg) &&
      req.getAddi() != "") { // ERR_CHANOPRIVSNEEDED
  } else {
    Channel &channel = channel_map[param[0]];
    if (req.getAddi() != "") { // change topic
      msg  = std::string(":") + client.getIdentify() + " ";
      msg += req.getCommand() + " ";
      msg += param[0] + " ";
      msg += req.getAddi() + "\r\n";
      UMstring_bool &joined_client = channel.getJoinedClient();
      UMstring_bool::iterator i;
      for (i = joined_client.begin(); i != joined_client.end(); ++i) {
        send_(serv_, serv_.getClient(i->first), msg, "response: topic: ");
      }
      std::string const &addi = req.getAddi();
      if (addi == "\"\"") {
        channel.setTopic("");
      } else {
        channel.setTopic(addi);
      }
      return;
    } else if (channel.getTopic() == "") { // RPL_NOTOPIC
      msg += "331 ";
      msg += param[0] + " ";
      msg += ":No topic is set\r\n";
    } else { // RPL_TOPIC  
      msg += "332 ";
      msg += param[0] + " ";
      msg += channel.getTopic() + "\r\n";
    }
  }
  send_(serv_, client, msg, "response: topic: ");
}

void RequestCallback::mode(Request const &req, RequestPool &requests) {
  (void) requests;
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client, req.getCommand())) { return; }
  Vstring const &param = req.getParam();
  std::string msg = std::string(":") + serv_.getHost() + " ";
  if (!errNeedMoreParam(2, req.getCommand(), param, msg)) { // ERR_NEEDMOREPARAM
  } else if (!channelCheck(serv_, param[0], msg)) { // ERR_NOSUCHCHANNEL
  } else if (!isChannelOper(serv_, client, param[0], msg)) {// ERR_CHANOPRIVSNEEDED
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
      } else if (!serv_.getChannelMap()[param[0]].isJoined(param[2])) {
        // ERR_USERNOTINCHANNEL
        msg += "441 ";
        msg += client.getNick() + " ";
        msg += param[0] + " ";
        msg += ":They aren't on that channel\r\n";
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
        msg += "324 ";
        for (int i = 0; i < (int)param.size() - 1; ++i) {
          msg += param[i] + " ";
        }
        channel.setHasLimit(param[1][0] == '+');
        if (param[1][0] == '+') {
          size_t size = string_to<size_t>(param[2]);
          channel.setLimit(size);
          msg += std::to_string(size) + " ";
        }
        msg += "\r\n";
      }
    }
  }
  send_(serv_, client, msg, "response: mode: ");
}

void RequestCallback::quit(Request const &req, RequestPool &requests) {
  (void) requests;
  Client &client = serv_.getClient(req.getRequesterSocket());
  if (!verify(serv_, client, req.getCommand())) { return; }
  // Vstring const &param = req.getParam();
  std::string msg = std::string(":") + client.getIdentify() + " ";
  msg += req.getCommand() + " ";
  msg += req.getAddi() + "\r\n";
  send_(serv_, client, msg, "response: quit: ");
  serv_.disconnect(client.getSocket());
}

} // namespace irc
