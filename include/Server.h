/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkong <tkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 22:17:37 by tkong             #+#    #+#             */
/*   Updated: 2023/11/29 22:17:37 by tkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_SERVER_H_
#define IRCSERV_SERVER_H_

#include <unordered_map>
#include <string>

#include "Client.h"
#include "Channel.h"
#include "RequestCallback.h"

namespace irc {

class Request;
class RequestPool;

typedef std::unordered_map<int, Client> UMint_Client; // connection_
typedef std::unordered_map<std::string, int> UMstring_int; // nick_to_sock_
typedef std::unordered_map<std::string, Channel> UMstring_Channel; // channel_map_

class Server {
 public:
  Server();
  ~Server() {}

  int getSocket() const;
  std::string const &getHost() const;
  UMint_Client &getConnection();
  UMstring_int &getNickToSock();
  UMstring_Channel &getChannelMap();
  Client &getClient(int sock);
  Client &getClient(std::string const &nick);
  void setPort(int port);
  void setPassword(std::string const &password);
  bool verify(std::string const &password) const;

  void standby();
  void preProcess();
  int accept();
  void disconnect(int sock);
  void disconnect(std::string const &nick);

  void response(Request const &req, RequestPool &requests);

 private:
  int         sock_;
  int         port_;
  std::string password_;
  std::string host_;

  UMint_Client     connection_;
  UMstring_int     nick_to_sock_;
  UMstring_Channel channel_map_;

  RequestCallback request_callback_;
};

} // namespace irc

#endif // IRCSERV_SERVER_H_
