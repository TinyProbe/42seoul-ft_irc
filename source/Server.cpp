#include "Server.hpp"

namespace irc {

static int const kMaxBacklog = 128;

int Server::getSocket() const {
  return serv_sock_;
}

Connections &Server::getConnections() const {
  return connections_;
}

Client &Server::getClient(int socket) const {
  Connections::iterator it = connections_.find(socket);
  if (it == connections_.end()) {
    throw std::runtime_error(std::string("getClient: invalid key"));
  }
  return it->second;
}

Client &Server::getClient(std::string const &nick) const {
  NickToSock::iterator it = nick_to_sock_.find(nick);
  if (it == nick_to_sock_.end()) {
    throw std::runtime_error(std::string("getClient: invalid key"));
  }
  return connections_[it->second];
}

void Server::setPort(int port) {
  if (port < 0 || port > 65535) {
    throw std::runtime_error(std::string("port: ") +
                             std::string(strerror(errno)));
  }
  port_ = port;
}

void Server::setPassword(std::string const &password) {
  if (password.size() < 6 || password.size() > 16) {
    throw std::runtime_error(std::string("password: 6 <= password >= 16"));
  }
  for (int i = 0; i < password.size(); ++i) {
    if (!isprint(password[i])) {
      throw std::runtime_error(
          std::string("password: format error(unprintable)"));
    }
  }
  password_ = password;
}

bool Server::verify(std::string const &password) const {
  return password_ == password;
}

void Server::standby() {
  if (serv_sock_ != -1) { close(serv_sock_); }

  if ((serv_sock_ = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    throw std::runtime_error(std::string("socket: ") +
                             std::string(strerror(errno));
  }
  fcntl(serv_sock_, F_SETFL, O_NONBLOCK);
  // for port release when shutdown the server program
  // setsockopt(serv_sock_, SOL_SOCKET, SO_REUSEADDR, { 1 }, sizeof(int));

  struct sockaddr_in serv_addr;
  bzero((void *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port        = htons(port_);

  if (bind(serv_sock_,
           (struct sockaddr *) &serv_addr,
           sizeof(serv_addr)) == -1) {
    throw std::runtime_error(std::string("bind: ") +
                             std::string(strerror(errno));
  }

  if (listen(serv_sock_, kMaxBacklog) == -1) {
    throw std::runtime_error(std::string("listen: ") +
                             std::string(strerror(errno));
  }
}

int Server::accept() {
  int socket;
  struct sockaddr_in serv_addr;
  bzero((void *) &serv_addr, sizeof(serv_addr));
  if ((socket = accept(serv_sock_,
                       (struct sockaddr *) &serv_addr,
                       sizeof(serv_addr))) == -1) {
    if (errno == EWOULDBLOCK) {
      std::cerr << std::string("accept: ") +
                   std::string(strerror(errno) << std::endl;
    } else {
      throw std::runtime_error(std::string("accept: ") +
                               std::string(strerror(errno)));
    }
  }
  fcntl(socket, F_SETFL, O_NONBLOCK);
  connections_[socket].setAddress(serv_addr.sin_addr.s_addr);
  return socket;
}

void Server::preProcess() {
  static Connections::iterator i;
  for (i = connections_.begin(); i != connections_.end(); ++i) {
    i->second.setWrite(false);
  }
  // timeout check
}

void Server::disconnect(int socket) {
  Connections::iterator it = connections_.find(socket);
  if (it != connections_.end()) {
    close(it->first);
    nick_to_sock_.erase(it->second.getNickname());
    connections_.erase(it);
  }
}

void Server::disconnect(std::string const &nick) {
  NickToSock::iterator it = nick_to_sock_.find(nick);
  if (it != nick_to_sock_.end()) {
    close(it->second);
    connections_.erase(it->second);
    nick_to_sock_.erase(it);
  }
}

Response const &Server::response(Request const &req) {
  return request_callbacks_(req);
}

bool Server::perform(Response const &res) {
  return response_callbacks_(res);
}

} // namespace irc
