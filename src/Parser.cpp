#include "Parser.hpp"

Parser::Parser(char *fileName) {
	//Тут нужно распарсить конфиг и залить всю инфу в ServerConfigs
	(void)fileName;
	// <Заглушка>
	ServerConfig confTmp;
	confTmp.setPort(7554);
	_serverConfigs.push_back(confTmp);
	//confTmp.setPort(7553);
	//_serverConfigs.push_back(confTmp);
	// </Заглушка>
}

Parser::Parser(const Parser &other){
	*this = other;
}

Parser::~Parser() {
}

Parser &		Parser::operator=( const Parser &other){
	if (this != &other) {
		;
	}
	return (*this);
}

const std::vector<ServerConfig> &Parser::getServerConfigs() const {
	return _serverConfigs;
}
