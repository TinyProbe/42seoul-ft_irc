/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Program.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkong <tkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 22:17:36 by tkong             #+#    #+#             */
/*   Updated: 2023/11/29 22:17:36 by tkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Program.h"

#include <iostream>

#include "common.h"

namespace irc {

void Program::run(int argc, char **argv) {
  if (argc != 3) {
    throw std::runtime_error("./ircserv <port> <password>");
  }
  init(argv);
  loop();
}

void Program::init(char **argv) {
  serv_.setPort(string_to<int>(argv[1]));
  serv_.setPassword(argv[2]);
  serv_.standby();

  events_.changeEvent(serv_.getSocket(), EVFILT_READ, EV_ADD);
  events_.setCapacity(5);

  signal(SIGPIPE, sigpipe);
}

void Program::loop() {
  struct kevent ev;
  Request req;
  while (true) {
    serv_.preProcess();
    // create Request
    while (events_.pollEvent(ev)) { request(ev); }
    // response and create request
    int size = requests_.getSize();
    while (size--) {
      requests_.pollRequest(req);
      response(req);
    }
  }
}

void Program::request(struct kevent const &ev) {
  if (ev.flags & EV_ERROR) { // error process
    if (static_cast<int>(ev.ident) == serv_.getSocket()) {
      throw std::runtime_error(std::string("event: server: ") +
                               std::string(strerror(errno)));
    } else {
      serv_.disconnect(ev.ident);
      std::cerr << std::string("event: client: ") +
                   std::string(strerror(errno)) << std::endl;
    }
    return;
  }

  if (static_cast<int>(ev.ident) == serv_.getSocket()) { // server
    if (ev.filter == EVFILT_READ) {
      int sock = serv_.accept();
      events_.changeEvent(sock, EVFILT_READ, EV_ADD);
      events_.changeEvent(sock, EVFILT_WRITE, EV_ADD);
      events_.setCapacity(serv_.getConnection().size() * 2 + 5);
    }
  } else { // client
    Client &client = serv_.getClient(ev.ident);
    if (ev.filter == EVFILT_READ) {
      if (client.receive()) {
        while (client.makeRequest()) {
          requests_.push(client.getRequest());
        }
      } else {
        serv_.disconnect(client.getSocket());
      }
    } else if (ev.filter == EVFILT_WRITE) {
      client.setWrite(true);
    }
  }
}

void Program::response(Request const &req) {
  try { serv_.response(req, requests_); }
  catch (...) { requests_.push(req); }
}

} // namespace irc
