#ifndef SERVER_SERVERCONFIG_HPP
# define SERVER_SERVERCONFIG_HPP
# include <iostream>
# include <netinet/in.h>
# include <map>
# include <string.h>

class ServerConfig {
private:
	int						_port;
	std::string				_host;

public:
	ServerConfig();
	ServerConfig(const ServerConfig &);
	virtual ~ServerConfig();
	ServerConfig		&operator=(const ServerConfig &);
	int					getPort() const;
	const std::string	&getHost() const;
	void				setPort(int port);
	void				setHost(const std::string &host);

};



#endif //SERVER_SERVERCONFIG_HPP
