/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestCallback.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkong <tkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 22:16:59 by tkong             #+#    #+#             */
/*   Updated: 2023/11/29 22:16:59 by tkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_REQUESTCALLBACK_H_
#define IRCSERV_REQUESTCALLBACK_H_

#include <vector>
#include <string>

namespace irc {

class Server;
class Request;
class RequestPool;

typedef std::vector<std::string> Vstring;

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
  void privMsg(Request const &req, RequestPool &requests); // derive: privMsg
  void join(Request const &req, RequestPool &requests); // derive: join   add: topic, names, part
  void names(Request const &req, RequestPool &requests);
  void part(Request const &req, RequestPool &requests);
  void kick(Request const &req, RequestPool &requests);
  void invite(Request const &req, RequestPool &requests); 
  void accept(Request const &req, RequestPool &requests); // add: join
  void deny(Request const &req, RequestPool &requests);
  void topic(Request const &req, RequestPool &requests);
  void mode(Request const &req, RequestPool &requests);
  void quit(Request const &req, RequestPool &requests);

  Server &serv_;
};

} // namespace irc

#endif // IRCSERV_REQUESTCALLBACK_H_
