//
// Created by enoelia on 08.06.2021.
//

#ifndef SERVER_RESPONSE_HPP
# define SERVER_RESPONSE_HPP
# include <iostream>
# include <fstream>
# include "RequestHandler.hpp"

class Response {
private:
	std::string		_serverAnswer;
	std::string		_headers;
	std::string		_body;
	std::string		_contentLength;

public:
	Response();
	Response(const Response &);
	virtual ~Response();
	Response &operator=(const Response &);

	void				setServerAnswer(const std::string &serverAnswer);

	void				setUpHeaders();
	void			 	setUpBody(std::stringstream &buffer);
	const std::string	receiveAnswer();
};

#endif //SERVER_RESPONSE_HPP
