#include "Cluster.hpp"

Cluster::Cluster(ParseConfig *parser) : _maxFd(0){
	for (int j = 0; j <= parser->getPosServ(); j++) {
		Server *newServer = new Server;
		struct sockaddr_in socketAddrTmp;
		std::vector<ParseConfig *> servInfo = parser->getServInfo();
		std::vector<t_location *> tmpVector;
		std::map<std::string, std::string> errors = servInfo[j]->getErrorPages();
		std::string *tmpServerErrorPaths = newServer->getErrorPaths();

		std::map<std::string, std::string> &head_fields = servInfo[j]->getMapHeadFields();
		std::vector<std::string> locations_sections = servInfo[j]->getValueLocPath();
		std::map<std::string, std::map<std::string, std::string> > locations_info = servInfo[j]->getMapLoc();
		std::map<int, std::map<int, std::string> > methods = servInfo[j]->getMethods();

		newServer->setServerName(head_fields["server_name"]);
		newServer->setHost(head_fields["host"]);
		socketAddrTmp.sin_family = AF_INET;
		socketAddrTmp.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("192.168.20.38");
		socketAddrTmp.sin_port = htons(newServer->checkPort(head_fields["port"]));
		newServer->setUpMaxBodySize(head_fields["max_body_size"]);

		tmpServerErrorPaths[ERR400] = errors["400"];
		tmpServerErrorPaths[ERR403] = errors["403"];
		tmpServerErrorPaths[ERR404] = errors["404"];
		tmpServerErrorPaths[ERR405] = errors["405"];
		tmpServerErrorPaths[ERR408] = errors["408"];
		tmpServerErrorPaths[ERR505] = errors["505"];

		for (int i = 0; i < locations_sections.size(); i++){
			t_location *tmpLocation = new t_location;
			tmpVector.push_back(tmpLocation);
			tmpLocation->url = locations_sections[i];
			tmpLocation->root = locations_info[locations_sections[i]]["root"];
			tmpLocation->index = locations_info[locations_sections[i]]["index"];
			tmpLocation->cgi_extension = locations_info[locations_sections[i]]["cgi_extension"];
			tmpLocation->autoindex = locations_info[locations_sections[i]]["autoindex"] == "on"? 1: 0;
			tmpLocation->methods[GET] = 0;
			tmpLocation->methods[POST] = 0;
			tmpLocation->methods[DELETE] = 0;
			tmpLocation->methods[PUT] = 0;
			for (int a = 0; a < METHODS_COUNT; a++) {
				if (methods[i][a] == std::string("GET")) {
					tmpLocation->methods[GET] = 1;
				} else if (methods[i][a] == std::string("POST")) {
					tmpLocation->methods[POST] = 1;
				} else if (methods[i][a] == std::string("DELETE")) {
					tmpLocation->methods[DELETE] = 1;
				} else if (methods[i][a] == std::string("PUT")) {
					tmpLocation->methods[PUT] = 1;
				} else if (methods[i][a] == std::string("")) { //what the fuck??
					;
				} else {
					throw Exceptions::MethodsException();
				}
			}
		}
		newServer->setLocations(tmpVector);
		newServer->setSockAddr(socketAddrTmp);
		newServer->createListenSocket();
		_servers.push_back(newServer);
	}
}

Cluster::~Cluster() {
	for (std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		delete (*it);
	}
}

const std::vector<Server*>	&Cluster::getServers() const {
	return _servers;
}

void						Cluster::resetSockets(){
	resetFdSets();
	for (std::vector<Server*>::const_iterator it = _servers.begin() ; it != _servers.end(); ++it){
		(*it)->getListenSocketFd() > _maxFd ? _maxFd = (*it)->getListenSocketFd() : 0;
		(*it)->resetListenSocket(_readFds);
		for (std::vector<Connection*>::const_iterator  itt = (*it)->getConnections().begin() ; itt != (*it)->getConnections().end(); ++itt){
			if ((*itt)->getStatus() == READ){
				std::cout << "new sockets to read " << (*itt)->getSocketFd() << std::endl;
				FD_SET((*itt)->getSocketFd(), &_readFds);
			}
			if ((*itt)->getStatus() == WRITE){
				std::cout << "new sockets to write " << (*itt)->getSocketFd() << std::endl;
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
	for (std::vector<Server*>::iterator it = _servers.begin() ; it != _servers.end(); ++it){
		if (FD_ISSET((*it)->getListenSocketFd(), &_readFds)){
			(*it)->acceptConnection();
		}
	}
}

void							Cluster::readFromSockets() {
	for (std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		(*it)->readFromSockets(_readFds);
	}
}

void							Cluster::writeToSockets() {
	for (std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		(*it)->writeToSockets(_writeFds);
	}
}

void 							Cluster::deleteClosedConnections() {
	for (std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		(*it)->deleteClosedConnections();
	}
}