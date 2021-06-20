#ifndef SERVER_SERVER_HPP
# define SERVER_SERVER_HPP
# include <iostream>
# include <netinet/in.h>
# include <vector>
# include "Connection.hpp"
# include "Exceptions.hpp"

class Connection;

class Server {
private:
	int 					_listenSocketFd;
	struct sockaddr_in		_socketAddr;
	std::vector<Connection*> _connections;
	std::string				_404path;

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
	const std::string &				get404Path() const;
	void							set404Path(const std::string &);

	void						 	createListenSocket();
	void							resetListenSocket(fd_set &readFds) const;
	void 							acceptConnection();
	void 							readFromSockets(fd_set);
	void 							writeToSockets(fd_set);

};


#endif