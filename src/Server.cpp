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
	for (std::vector<t_location *>::iterator it = _locations.begin(); it != _locations.end(); ++it) {
		delete *it;
	}
}

Server&							Server::operator=( const Server &other){
	if (this != &other) {
		_listenSocketFd = other._listenSocketFd;
		_socketAddr = other._socketAddr;
		_connections = other._connections;
		_serverName = other._serverName;
		_locations = other._locations;
		_max_body_size = other._max_body_size;
		_connections = other._connections;
		_host = other._host;
		_404path = other._404path;
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

const std::string &Server::get404Path() const {
	return _404path;
}

void Server::set404Path(const std::string & path){
	_404path = path;
}

std::string Server::getServerName() const {
	return _serverName;
}

std::string Server::getHost() const {
	return _host;
}

void Server::setHost(std::string &host) {
	_host = host;
}

void Server::setServerName(std::string &serverName) {
	_serverName = serverName;
	serverName.empty()? throw Exceptions::ServerNameException(): _serverName = serverName;
}

void 							Server::acceptConnection(){
	_connections.push_back(new Connection(_listenSocketFd, this));
}

void 							Server::readFromSockets(fd_set readFds){
	for (std::vector<Connection*>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
		if (FD_ISSET((*it)->getSocketFd(), &readFds)) {
			(*it)->readFromSocket();
		}
	}
}
void 							Server::writeToSockets(fd_set writeFds){
	for (std::vector<Connection*>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
		if (FD_ISSET((*it)->getSocketFd(), &writeFds)) {
			(*it)->writeToSocket();
		}
	}
}

int							Server::checkPort(const std::string &parserAnswer){
	int port;
	if (parserAnswer.empty() || parserAnswer.length() > 5) {
		throw Exceptions::PortException();
	}
	std::istringstream(parserAnswer) >> port;
	if (port < 0 || port > 65535){
		throw Exceptions::PortException();
	}
	return port;
}

long long int Server::getMaxBodySize() const {
	return _max_body_size;
}

void Server::setUpMaxBodySize(std::string &parserAnswer){
	if (parserAnswer.empty() || parserAnswer[0] == '-'){
		throw Exceptions::MaxBodySizeException();
	}
	std::istringstream(parserAnswer) >> _max_body_size;
}

std::vector<t_location*> &Server::getLocations(){
	return _locations;
}

void Server::setLocations(const std::vector<t_location*> &locations) {
	_locations = locations;
}