#include "Cluster.hpp"

Cluster::Cluster(ParseConfig *parser) : _maxFd(0){
	Server newServer;
	struct sockaddr_in socketAddrTmp;

	std::vector <ParseConfig *> servInfo = parser->getServInfo();
	for (int j = 0; j <= parser->getPosServ(); j++)
	{
		t_location *ptr;
		std::map <std::string, std::string> &head_fields = servInfo[j]->getMapHeadFields();
		std::vector <std::string> locations_sections = servInfo[j]->getValueLocPath();
		std::map < std::string, std::map<std::string, std::string> > locations_info = servInfo[j]->getMapLoc();
		std::map < int, std::map<int, std::string> > methods = servInfo[j]->getMethods();

		newServer.setServerName(head_fields["server_name"]);
		newServer.setHost(head_fields["host"]);
		socketAddrTmp.sin_family = AF_INET;
		socketAddrTmp.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("192.168.20.38");
		socketAddrTmp.sin_port = htons(newServer.checkPort(head_fields["port"]));
		newServer.setUpMaxBodySize(head_fields["max_body_size"]);
		newServer.setLocations(new t_location[locations_sections.size()]);
		ptr = newServer.getLocations();
		for (int i = 0; i < locations_sections.size(); i++){
			ptr[i].url = locations_sections[i];
			ptr[i].root = locations_info[locations_sections[i]]["root"];
			ptr[i].index = locations_info[locations_sections[i]]["index"];
			ptr[i].cgi_extension = locations_info[locations_sections[i]]["cgi_extension"];
			ptr[i].autoindex = locations_info[locations_sections[i]]["autoindex"] == "on"? 1: 0;
			ptr[i].methods[0] = 0;
			ptr[i].methods[1] = 0;
			ptr[i].methods[2] = 0;
			for (int a = 0; a <= 3; a++) {
				if (methods[i][a] == std::string("GET")) {
					ptr[i].methods[0] = 1;
				} else if (methods[i][a] == std::string("POST")) {
					ptr[i].methods[1] = 1;
				} else if (methods[i][a] == std::string("DELETE")) {
					ptr[i].methods[2] = 1;
				} else if (methods[i][a] == std::string("")) { //what the fuck??
					;
				} else {
					throw Exceptions::MethodsException();
				}
			}
		}
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