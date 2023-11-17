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
    // Connections &connections = server_.getConnections();
    // Connections::iterator i;
    // for (i = connections.begin(); i != connections.end(); ++i) {
    //   close(i->first);
    // }
    // int socket = server_.getSocket();
    // if (socket != -1) {
    //   close(socket);
    // }
    throw e;
  }
}

void Program::init(int argc, char **argv) {
  (void)argc;
  server_.setPort(std::string(argv[1]));
  server_.setPassword(std::string(argv[2]));
  server_.standby();

  events_.changeEvent(server_.getSocket(), EVFILT_READ, EV_ADD);
}

void Program::loop() {
  struct kevent ev;
  Request req;
  Response res;
  while (true) {
    server_.preProcess(); // setWrite(false), recognize timeout check
    while (events_.pollEvent(ev)) { 
      request(ev);
      while (requests_.pollRequest(req)) { response(req); }
      while (responses_.pollResponse(res)) { perform(res); }
    }
    // while (requests_.pollRequest(req)) { response(req); }
    // while (responses_.pollResponse(res)) { perform(res); }
  }
}

void Program::request(struct kevent const &ev) {
  if (ev.flags & EV_ERROR) {
    if (static_cast<int>(ev.ident) == server_.getSocket()) {
      throw std::runtime_error(std::string("server: ") +
                               std::string(strerror(errno)));
    } else {
      server_.disconnect(ev.ident);
      std::cerr << std::string("client: ") +
                   std::string(strerror(errno)) << std::endl;
    }
    return;
  }

  if (static_cast<int>(ev.ident) == server_.getSocket()) {
    if (ev.filter == EVFILT_READ) {
      int socket = server_.accept();
      events_.changeEvent(socket, EVFILT_READ, EV_ADD);
      events_.changeEvent(socket, EVFILT_WRITE, EV_ADD);
    }
  } else {
    Client &client = server_.getClient(ev.ident);
    if (ev.filter == EVFILT_READ) {
      client.receive(ev.ident);
      client.certification();
      if (client.canRequest()) {
        requests_.push(client.createRequest());
      }
    } 
    else if (ev.filter == EVFILT_WRITE) {
      client.setWrite(true);
    }
  }
}

void Program::response(Request const &req) {
  int code = req.getRequestCode();
  std::cout << "code : " << code <<std::endl;
  if (code == RECOGNIZE) {
    std::cout << "!23\n";
    Client &client = server_.getClient(req.getSocket());
    if (client.getPassword() != server_.getPassword()) {
      responses_.push(Response(Command::wrongpass(req, client.getPassword()), EXIT, req.getSocket()));
    }
    else if (server_.samename(client.getNickname()) == 2) {
      responses_.push(Response(Command::nicknameinuse(req, client.getNickname()), EXIT, req.getSocket()));
    }
    else
      responses_.push(Response(Command::welcome(req), code, req.getSocket()));
  } 
  else if (code == JOIN) {
    std::string name = Command::firstword(req.getType());
    std::string password = Command::secondword(req.getType());

    if (!server_.setChannel(name, password, req.getNickname())) {
      Channel &channel = server_.getChannel(name);
      if (password != channel.getPassword()) {
        responses_.push(Response(Command::wrongpass(req, password), code, req.getSocket()));
        return ;
      }
      // else if (checkBan(name)) {

      // }
      channel.inputCh(req.getNickname());
    }
    responses_.push(Response(Command::join(req), code, req.getSocket()));
  }
  else if (code == NICK) {
    std::string nick = Command::firstword(req.getType());
    if (server_.samename(nick) == 1) {
      responses_.push(Response(Command::nicknameinuse(req, nick), code, req.getSocket()));
      return ;
    }
    else if (nick == ":") {
      responses_.push(Response(Command::NoNickname(req), code, req.getSocket()));
      return ;
    }
    Client &client = server_.getClient(req.getSocket());
    client.setNickname(nick);
    responses_.push(Response(Command::ChangeNick(req), code, req.getSocket()));
  }
  else if (code == PRIVMSG) {
    std::string nick = Command::firstword(req.getType());
    if (nick[0] == '#') {
      if (!server_.canChannel(nick)) {
      responses_.push(Response(Command::nosuchchannel(req, nick), code, req.getSocket()));
      return ;
    }
      Channel &channel = server_.getChannel(nick);
      std::vector<std::string> Ch_nick_ = channel.getChNick();
      Connections &connection = server_.getConnections();
      Connections::iterator citer;

      if(find(Ch_nick_.begin(), Ch_nick_.end(), req.getNickname()) == Ch_nick_.end()) { // vector내에 0이 존재하지 않음
        responses_.push(Response(Command::NotChannelIn(req, req.getNickname(), nick), code, req.getSocket()));
        return ;
    }
      for(size_t i = 0; i != Ch_nick_.size(); ++i) {
        for (citer = connection.begin(); citer != connection.end(); ++citer) {
          if (citer->second.getNickname() == Ch_nick_[i] && Ch_nick_[i] != req.getNickname()) {
            responses_.push(Response(Command::Privmsg(req), code, citer->first));
          }
        }
      }
      return ;
    }
    else if (server_.samename(nick) == 0) {
      responses_.push(Response(Command::nosuchnick(req, nick), code, req.getSocket()));
      return ;
    }
    responses_.push(Response(Command::Privmsg(req), code, req.getSocket()));
  }
  else if (code == KICK) {
    std::string chname = Command::firstword(req.getType());
    std::string nick = Command::secondword(req.getType());

    if (!server_.canChannel(chname)) {
      responses_.push(Response(Command::nosuchchannel(req, chname), code, req.getSocket()));
      return ;
    }
    else if (server_.samename(nick) == 0) {
      responses_.push(Response(Command::nosuchnick(req, nick), code, req.getSocket()));
      return ;
    }
    else {
      Channel &channel = server_.getChannel(chname);
      std::vector<std::string> Ch_nick_ = channel.getChNick();
      std::cout << "Channel name : " << nick << std::endl;
      for (size_t i = 0; i != Ch_nick_.size(); ++i) {
        std::cout << "Channel name : " << Ch_nick_[i] << std::endl;
        if (nick == Ch_nick_[i]) {
          channel.Channelout(nick);
          server_.mangeCh(chname);
          responses_.push(Response(Command::kick(req), code, req.getSocket()));
          responses_.push(Response(Command::kick(req), code, server_.nicktosocket(nick)));
          return ;
        }
      }
      responses_.push(Response(Command::NotChannelIn(req, nick, chname), code, req.getSocket()));
    }
  }
  else if (code == PART) {
    std::string chname = Command::firstword(req.getType());
    Channel &channel = server_.getChannel(chname);

    channel.Channelout(req.getNickname());
    server_.mangeCh(chname);
    responses_.push(Response(Command::kick(req), code, req.getSocket()));
  }
  else if (code == QUIT) {
    responses_.push(Response(Command::kick(req), EXIT, req.getSocket()));
  }
  else if (code == TOPIC) {
    std::string chname = Command::firstword(req.getType());
    std::string topic = Command::secondword(req.getType());

    if (!server_.canChannel(chname)) {
      responses_.push(Response(Command::nosuchchannel(req, chname), code, req.getSocket()));
      return ;
    }
    else if (topic.empty()) {
      responses_.push(Response(Command::topic(chname, server_.getChTopic(chname)), code, req.getSocket()));
    }
    else {
      server_.setChTopic(chname, topic);
      responses_.push(Response(Command::kick(req), code, req.getSocket()));
      return ;
    }
  }
  else {
    responses_.push(Response(Command::unknowncommand(req, req.getType()), code, req.getSocket()));
  }
}

void Program::perform(Response const &res) {
  char *tmp;

  tmp = const_cast<char*>(res.response_.c_str());
  if (res.type_ == EXIT) {
    Connections &connection = server_.getConnections();
    connection.erase(res.socket_);
    events_.changeEvent(res.socket_, EVFILT_READ, EV_DELETE);
    events_.changeEvent(res.socket_, EVFILT_WRITE, EV_DELETE);
    close(res.socket_);
  }
  else {
    std::cout << "send mess : " << tmp << std::endl;
    send(res.socket_, tmp, strlen(tmp), 0);
  }
}

} // namespace irc
