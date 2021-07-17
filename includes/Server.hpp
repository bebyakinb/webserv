#ifndef SERVER_SERVER_HPP
# define SERVER_SERVER_HPP
# include <iostream>
# include <netinet/in.h>
# include <vector>
# include "Connection.hpp"
# include "Exceptions.hpp"
# include "Defenitions.hpp"
class Connection;

typedef struct s_location{
	std::string		url;
	std::string		root;
	std::string		index;
	int				methods[METHODS_COUNT];
	std::string		cgi_path;
	int				autoindex;
} t_location;

class Server {
private:
	int 						_listenSocketFd;
	struct sockaddr_in			_socketAddr;
	std::vector<Connection*>	_connections;
	std::string					_serverName;
	std::string					_host;
	long long int				_max_body_size;
	std::vector<t_location*>	_locations;
	std::string					_errorPaths[ERRS_COUNT];

public:
	Server();
	Server(const Server &);
	virtual ~Server();
	Server		&operator=(const Server &);

	void 							setSockAddr(const sockaddr_in &sockAddr);
	void 							setListenSocketFd(int fd);
	const sockaddr_in&				getSocketAddr() const;
	int								getListenSocketFd() const;
	const std::vector<Connection*>&	getConnections() const;
	std::string						getServerName() const;
	void							setServerName(std::string &serverName);
	std::string						getHost() const;
	void							setHost(std::string &host);
	long long int					getMaxBodySize() const;
	std::string 					*getErrorPaths();
	std::vector<t_location*>&		getLocations();
	void							setLocations(const std::vector<t_location*> &locations);

	void						 	createListenSocket();
	void							resetListenSocket(fd_set &readFds) const;
	void 							acceptConnection();
	void 							readFromSockets(fd_set);
	void 							writeToSockets(fd_set);
	void							deleteClosedConnections();
	int 							checkPort(const std::string &parserAnswer);
	void							setUpMaxBodySize(std::string &parserAnswer);


};


#endif