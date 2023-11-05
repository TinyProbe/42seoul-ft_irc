#include "Server.hpp"

namespace irc {

static int const kMaxBacklog = 128;

int Server::getSocket() const {
  return sock_;
}

Connections &Server::getConnection() const {
  return connection_;
}

Client &Server::getClient(int sock) const {
  Connections::iterator it = connection_.find(sock);
  if (it == connection_.end()) {
    throw std::runtime_error(std::string("getClient: invalid key"));
  }
  return it->second;
}

Client &Server::getClient(std::string const &nick) const {
  NickToSock::iterator it = nick_to_sock_.find(nick);
  if (it == nick_to_sock_.end()) {
    throw std::runtime_error(std::string("getClient: invalid key"));
  }
  return connection_[it->second];
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
  if (sock_ != -1) { close(sock_); }

  if ((sock_ = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    throw std::runtime_error(std::string("socket: ") +
                             std::string(strerror(errno));
  }
  fcntl(sock_, F_SETFL, O_NONBLOCK);
  // for port release when shutdown the server program
  // setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, { 1 }, sizeof(int));

  struct sockaddr_in serv_addr;
  bzero((void *) &serv_addr, sizeof(struct sockaddr_in));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port        = htons(port_);

  if (bind(sock_,
           (struct sockaddr *) &serv_addr,
           sizeof(struct sockaddr_in)) == -1) {
    throw std::runtime_error(std::string("bind: ") +
                             std::string(strerror(errno));
  }

  if (listen(sock_, kMaxBacklog) == -1) {
    throw std::runtime_error(std::string("listen: ") +
                             std::string(strerror(errno));
  }
}

void Server::preProcess() {
  static Connections::iterator i;
  for (i = connection_.begin(); i != connection_.end(); ++i) {
    i->second.setWrite(false);
  }
}

int Server::accept() {
  int client_sock;
  struct sockaddr_in client_addr;
  bzero((void *) &client_addr, sizeof(struct sockaddr_in));
  if ((client_sock = accept(sock_,
                            (struct sockaddr *) &client_addr,
                            sizeof(struct sockaddr_in))) == -1) {
    if (errno == EWOULDBLOCK) {
      std::cerr << std::string("accept: ") +
                   std::string(strerror(errno) << std::endl;
    } else {
      throw std::runtime_error(std::string("accept: ") +
                               std::string(strerror(errno)));
    }
  }
  fcntl(client_sock, F_SETFL, O_NONBLOCK);
  Client &client = connection_[client_sock];
  client.setAddress(ntohl(client_addr.sin_addr.s_addr));
  client.setPort(ntohs(client_addr.sin_port));
  return client_sock;
}

void Server::disconnect(int sock) {
  Connection::iterator it = connection_.find(sock);
  if (it != connection_.end()) {
    close(it->first);
    nick_to_sock_.erase(it->second.getNickname());
    connection_.erase(it);
  }
}

void Server::disconnect(std::string const &nick) {
  NickToSock::iterator it = nick_to_sock_.find(nick);
  if (it != nick_to_sock_.end()) {
    close(it->second);
    connection_.erase(it->second);
    nick_to_sock_.erase(it);
  }
}

std::vector<Response> const &Server::response(Request const &req) {
  return request_callback_(req);
}

bool Server::perform(Response const &res) {
  return response_callback_(res);
}

} // namespace irc
