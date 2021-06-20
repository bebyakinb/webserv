#ifndef WEBSERV_SERVER_HPP
# define WEBSERV_SERVER_HPP
# include <iostream>
# include <vector>
# include "ServerConfig.hpp"
# include "Server.hpp"
# include <netinet/in.h>
# include <sys/socket.h>
# include "errno.h"

class Cluster {
private:
	fd_set						_readFds;
	fd_set						_writeFds;
	int							_maxFd;
	std::vector<Server>			_servers;
	struct timeval				_timeout;

	Cluster();
	Cluster(const Cluster &);
	Cluster  &operator=(const Cluster &);
public:
	Cluster(const std::vector<ServerConfig> &serverConfigs);
	virtual ~Cluster();

	const std::vector<Server>	&getServers() const;
	void						resetSockets();
	void						resetFdSets();
	int							serversSelect();
	void						acceptConnections();
	void						readFromSockets();
	void 						writeToSockets();
	void						closeFds();
};


#endif