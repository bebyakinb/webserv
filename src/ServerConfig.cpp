#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {

}

ServerConfig::ServerConfig(const ServerConfig &other){
	*this = other;
}

ServerConfig::~ServerConfig() {
}

ServerConfig &		ServerConfig::operator=( const ServerConfig &other){
	if (this != &other) {
		_port = other._port;
		_host = other._host;
	}
	return (*this);
}

int ServerConfig::getPort() const {
	return _port;
}

const std::string &ServerConfig::getHost() const {
	return _host;
}

void ServerConfig::setPort(int port) {
	_port = port;
}

void ServerConfig::setHost(const std::string &host) {
	_host = host;
}

