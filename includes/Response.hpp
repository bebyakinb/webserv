//
// Created by enoelia on 08.06.2021.
//

#ifndef SERVER_RESPONSE_HPP
# define SERVER_RESPONSE_HPP
# include <iostream>
# include <fstream>
# include <sys/time.h>
# include "RequestHandler.hpp"

class Response {
private:
	std::string							_serverAnswer;
	std::string							_headers;
	std::string							_body;
	std::string							_contentLength;
	std::string							_contentType;
	std::map<std::string, std::string>	_mime_map;
	std::string							_date;

public:
	Response();
	Response(const Response &);
	virtual ~Response();
	Response &operator=(const Response &);

	void				setServerAnswer(const std::string &serverAnswer);
	void				setUpHeaders(const std::string &extension);
	void			 	setUpBody(std::stringstream &buffer);
	const std::string	receiveAnswer();
	void				setUpContentType(const std::string &extension);
	void				setDate();


};

#endif //SERVER_RESPONSE_HPP
