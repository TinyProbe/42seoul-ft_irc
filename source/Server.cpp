# include "Server.hpp"

void Server::setPort(std::string port) {
	int port_num = 0;
	int size;

	for (size = 0; size < port.size(); size++) {
		if (port.size() > 5 || !isdigit(port[size]))
			break ;
		port_num = port_num * 10 + static_cast<int>(port[size]) - 48;
	}
	if (size != port.size() || port_num > 65535)
		throw std::runtime_error(std::string("port error\n"));
	port_ = port_num;
}

void Server::setPassword(std::string password) {
	int size;

	for (size = 0; size < password.size(); size++) {
		if (!isprint(password[i]))
			break ;
	}
	if (size != password.size())
		throw std::runtime_error(std::string("password error\n"));
	password_ = password;
}

int Server::getSocket(void) const {
	return server_socket_;
}

Connections &Server::getConnections(void) const {
	return connections_;
}

Client &Server::getClient (int socket) const {
	return connections_[socket];
}

void Server::standby(void) {
	int Server_socket;
	struct sockaddr_in server_addr;
	int flag = 1;
	
	if ((Server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error(std::string("socket() error\n") + std::string(strerror(errno)));
	setsockopt(Server_socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)); //프로그램이 종료되어도 8080port가 해제되지 않는 현상 없애기 위해 필요
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(Port_number_);
	if (bind(Server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		throw std::runtime_error(std::string("bind() error\n") + std::string(strerror(errno)));
	if (listen(Server_socket, MAX_BACKLOG) == -1)
		throw std::runtime_error(std::string("listen() error\n") + std::string(strerror(errno)));
	fcntl(Server_socket, F_SETFL, O_NONBLOCK);
	server_socket_ = Server_socket;
}

void Server::preProcess(void) {

}

void Server::disconnect(int socket) {
	nick_to_socket_.erase(connections_[socket].getNickname());
	connections_.erase(socket);
	close (socket);
}

int Server::accept(void) {
	int socket = accept(server_socket_, NULL, NULL);
	if (socket == -1) {
			throw std::runtime_error(std::string("accept() error\n") + std::string(strerror(errno)));
	}
	fcntl(socket, F_SETFL, O_NONBLOCK);
	connections_[socket];
	return socket;
}