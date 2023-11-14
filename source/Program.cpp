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
    Connection &connection = serv_.getConnection();
    Connection::iterator i;
    for (i = connection.begin(); i != connection.end(); ++i) {
      close(i->first);
    }
    if ((int sock = serv_.getSocket()) != -1) {
      close(sock);
    }
    throw e;
  }
}

void Program::init(int argc, char **argv) {
  serv_.setPort(string_to<int>(std::string(argv[1])));
  serv_.setPassword(std::string(argv[2]));
  serv_.standby();

  events_.changeEvent(serv_.getSocket(), EVFILT_READ, EV_ADD);
  events_.setCapacity(5);
}

void Program::loop() {
  struct kevent ev;
  Request req;
  Response res;
  int size;
  while (true) {
    serv_.preProcess();
    while (events_.pollEvent(ev)) { request(ev); }
    while (requests_.pollRequest(req)) { response(req); }
    size = responses_.getSize();
    while (size--) {
      responses_.pollResponse(res);
      perform(res);
    }
  }
}

void Program::request(struct kevent const &ev) {
  if (ev.flags & EV_ERROR) {
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

  if (static_cast<int>(ev.ident) == serv_.getSocket()) {
    if (ev.filter == EVFILT_READ) {
      int sock = serv_.accept();
      events_.changeEvent(sock, EVFILT_READ, EV_ADD);
      events_.changeEvent(sock, EVFILT_WRITE, EV_ADD);
      events_.setCapacity(serv_.getConnection().size() * 2 + 5);
    }
  } else {
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
  std::vector<Response> const &ress = serv_.response(req);
  for (int i = 0; i < ress.size(); ++i) {
    responses_.push(ress[i]);
  }
}

void Program::perform(Response const &res) {
  if (!serv_.perform(res)) { responses_.push(res); }
}

} // namespace irc
