#ifndef SERVER_PARSER_HPP
# define SERVER_PARSER_HPP
# include "ServerConfig.hpp"
# include <vector>

class Parser {
private:
	std::vector<ServerConfig> _serverConfigs;
	Parser();
public:
	Parser(char *fileName);
	Parser(const Parser &);
	virtual ~Parser();
	Parser		&operator=(const Parser &);

	const std::vector<ServerConfig> &getServerConfigs() const;
};


#endif //SERVER_PARSER_HPP
