#include "Program.h"

namespace irc {

void Program::run(int argc, char **argv) {
  if (argc != 3) {
    throw std::runtime_error("./ircserv <port> <password>");
  }
  try {
    init(argc, argv);
    loop();
  } catch (std::exception const &e) {
    Connections &connections = server_.getConnections();
    Connections::iterator i;
    for (i = connections.begin(); i != connections.end(); ++i) {
      close(i->first);
    }
    if ((int socket = server_.getSocket()) != -1) {
      close(socket);
    }
    throw e;
  }
}

void Program::init(int argc, char **argv) {
  server_.setPort(string_to<int>(argv[1]));
  server_.setPassword(std::string(argv[2]));
  server_.standby();

  events_.changeEvent(server_.getSocket(), EVFILT_READ, EV_ADD | EV_ENABLE);
}

void Program::loop() {
  struct kevent ev;
  Request req;
  Response res;
  while (true) {
    server_.preProcess(); // setWrite(false), recognize timeout check
    while (events_.pollEvent(ev)) { request(ev); }
    while (requests_.pollRequest(req)) { response(req); }
    while (responses_.pollResponse(res)) { perform(res); }
  }
}

void Program::request(struct kevent const &ev) {
  if (ev.flags & EV_ERROR) {
    if (static_cast<int>(ev.ident) == server_.getSocket()) {
      throw std::runtime_error(std::string("server: ") +
                               std::string(strerror(errno)));
    } else {
      close(ev.ident);
      server_.disconnect(ev.ident);
      std::cerr << std::string("client: ") +
                   std::string(strerror(errno)) << std::endl;
    }
    return;
  }

  if (static_cast<int>(ev.ident) == server_.getSocket()) {
    if (ev.filter == EVFILT_READ) {
      int socket = server_.accept();
      events_.changeEvent(socket, EVFILT_READ, EV_ADD | EV_ENABLE);
      events_.changeEvent(socket, EVFILT_WRITE, EV_ADD | EV_ENABLE);
    }
  } else {
    Client &client = server_.getClient(ev.ident);
    if (ev.filter == EVFILT_READ) {
      client.receive();
      if (client.canRequest()) {
        requests_.push(client.createRequest());
      }
    } else if (ev.filter == EVFILT_WRITE) {
      client.setWrite(true);
    }
  }
}

void Program::response(Request const &req) {
  int code = req.getRequestCode();
  if (code == Request::RECOGNIZE) {
  } else if (code == Request::) {
  } else if (code == Request::) {
  } else if (code == Request::) {
  } else if (code == Request::) {
  } else if (code == Request::) {
  }
}

void Program::perform(Response const &res) {
}

} // namespace irc
