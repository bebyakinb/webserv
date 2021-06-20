#include "Cluster.hpp"

Cluster::Cluster(const std::vector<ServerConfig> &serverConfigs) : _maxFd(0){
	for (std::vector<ServerConfig>::const_iterator it = serverConfigs.begin() ; it != serverConfigs.end(); ++it){
		Server newServer;
		struct sockaddr_in socketAddrTmp;

		socketAddrTmp.sin_family = AF_INET;
		socketAddrTmp.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("192.168.20.38");
		socketAddrTmp.sin_port = htons(it->getPort());
		newServer.setSockAddr(socketAddrTmp);
		newServer.createListenSocket();
		_servers.push_back(newServer);
	}
}

Cluster::~Cluster() {
}

const std::vector<Server>	&Cluster::getServers() const {
	return _servers;
}

void						Cluster::resetSockets(){
	resetFdSets();
	for (std::vector<Server>::const_iterator it = _servers.begin() ; it != _servers.end(); ++it){
		it->getListenSocketFd() > _maxFd ? _maxFd = it->getListenSocketFd() : 0;
		it->resetListenSocket(_readFds);
		for (std::vector<Connection*>::const_iterator  itt = it->getConnections().begin() ; itt != it->getConnections().end(); ++itt){
			if ((*itt)->getStatus() == READ){
				FD_SET((*itt)->getSocketFd(), &_readFds);
			}
			if ((*itt)->getStatus() == WRITE){
				FD_SET((*itt)->getSocketFd(), &_writeFds);
			}
			(*itt)->getSocketFd() > _maxFd ? _maxFd = (*itt)->getSocketFd() : 0;
		}
	}
}

void						Cluster::resetFdSets(){
	FD_ZERO(&_readFds);
	FD_ZERO(&_writeFds);
}

int							Cluster::serversSelect(){
	_timeout.tv_sec = 5;
	_timeout.tv_usec = 30000;
	return (select(_maxFd + 1, &_readFds, &_writeFds, NULL, &_timeout));
}

void						Cluster::acceptConnections(){
	for (std::vector<Server>::iterator it = _servers.begin() ; it != _servers.end(); ++it){
		if (FD_ISSET(it->getListenSocketFd(), &_readFds)){
			it->acceptConnection();
		}
	}
}

void							Cluster::readFromSockets() {
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		it->readFromSockets(_readFds);
	}
}

void							Cluster::writeToSockets() {
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		it->writeToSockets(_writeFds);
	}
}

void 							Cluster::closeFds() {
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		it->readFromSockets(_readFds);
	}
}