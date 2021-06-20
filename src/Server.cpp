#include "Server.hpp"

Server::Server() {

}

Server::Server(const Server &other){
	*this = other;
}

Server::~Server() {
	for (std::vector<Connection*>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
		delete *it;
	}
}

Server&							Server::operator=( const Server &other){
	if (this != &other) {
		_listenSocketFd = other._listenSocketFd;
		_socketAddr = other._socketAddr;
	}
	return (*this);
}

void							Server::setSockAddr(const sockaddr_in &sockAddr) {
	_socketAddr = sockAddr;
}

void Server::setListenSocketFd(int fd) {
	_listenSocketFd = fd;
}

const sockaddr_in&				Server::getSocketAddr() const {
	return _socketAddr;
}

void			 				Server::createListenSocket(){
	if ((_listenSocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
		throw Exceptions::SocketCreationException();
	}
	if (bind(_listenSocketFd, (struct sockaddr*)&_socketAddr, sizeof(_socketAddr)) == -1){
		throw Exceptions::BindException();
	}
}

void 							Server::resetListenSocket(fd_set &readFds) const{
		FD_SET(_listenSocketFd, &readFds);
		listen(_listenSocketFd, 16);
}

int								Server::getListenSocketFd() const {
	return _listenSocketFd;
}

const std::vector<Connection*>&	Server::getConnections() const {
	return _connections;
}

void 							Server::acceptConnection(){
	_connections.push_back(new Connection(_listenSocketFd, *this));
}

void 							Server::readFromSockets(fd_set readFds){
	for (std::vector<Connection*>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
		if (FD_ISSET((*it)->getSocketFd(), &readFds))
			(*it)->readFromSocket();
	}
}
void 							Server::writeToSockets(fd_set writeFds){
	for (std::vector<Connection*>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
		if (FD_ISSET((*it)->getSocketFd(), &writeFds))
			(*it)->writeToSocket();
	}
}

const std::string &Server::get404Path() const {
	return _404path;
}

void Server::set404Path(const std::string & path){
	_404path = path;
}
