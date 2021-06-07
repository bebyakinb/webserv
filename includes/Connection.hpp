#ifndef SERVER_CONNECTION_HPP
# define SERVER_CONNECTION_HPP
# define CLOSE 0
# define READ 1
# define WRITE 2
# define BUFFER_SIZE 65536
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <unistd.h>
# include <fstream>
# include <sstream>
# include "Errors.hpp"


class Connection {
private:
	int			_status;
	int			_socketFd;
	sockaddr_in	_addr;
	socklen_t	_addrlen;
	std::string	_request;

	Connection();
public:
	Connection(int listenSocketFd);
	Connection(const Connection &);
	virtual ~Connection();
	Connection		&operator=(const Connection &);

	int				getStatus() const;
	void			setStatus(int status);
	int				getSocketFd() const;
	void			setSocketFd(int fd);

	void			readFromSocket();
	void			writeToSocket();
	int				parseRequest();
};


#endif //SERVER_CONNECTION_HPP
