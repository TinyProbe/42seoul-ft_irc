# include "Server.hpp"

Server::Server(){

}

Server::~Server(){

}

void	Server::setPort(int port){
	port_number_ = port;
}

void	Server::setPassword(std::string Password){
	password_ = Password;
}

void	Server::standby(void){
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

int	Server::getSocket(void){
	return (server_socket_)
}

int Server::accept(void){
	int clt_sockfd = accept(serv_sockfd, NULL, NULL);
	if (clt_sockfd == -1) {
			throw std::runtime_error(std::string("accept() error\n") + std::string(strerror(errno)));
    }
	 fcntl(clt_sockfd, F_SETFL, O_NONBLOCK);
	 connections_[clt_sockfd];
	 return (clt_sockfd);
}

void	Server::disconnect(int ctlsock){
	Nicktoctl iterator it;

	for (it = nicktoctl_.begin(); it != nicktoctl_.end(); it++){
		if (it->second == ctlsock){
			nicktoctl_.earse(it->first);
			break ;
		}
	}
	connections_.earse(ctlsock);
}

Connections Server::getConnections(void){
	return (connections_);
}

Client Server::getClient(int ctl_sockfd){
	return (connections_[clt_sockfd]);
}