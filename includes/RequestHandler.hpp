#ifndef SERVER_REQUESTHANDLER_HPP
# define SERVER_REQUESTHANDLER_HPP
# include <iostream>
# include <fstream>
# include <map>
# include "Server.hpp"
# include "Response.hpp"
# define GET 1
# define POST 2
# define DELETE 3

class Server;
class Response;

class RequestHandler {
private:
	std::string							_rawRequest;
	Server*								_server;

	int									_method;
	std::string							_url;
	std::map<std::string,std::string>	_headers;

	std::string							_filePath;
	Response							*_response;
	std::string							_answer;
	unsigned long						_bytesToSend;

public:
	RequestHandler();
	RequestHandler(const RequestHandler &);
	virtual ~RequestHandler();
	RequestHandler	&operator=(const RequestHandler &);

	int					getMethod() const;
	void				setMethod(int method);
	Server				*getServer() const;
	void				setServer(Server *server);
	const				std::string &getRawRequest() const;
	void				setRawRequest(const std::string &rawRequest);
	const std::string& 	getAnswer() const;
	unsigned long		getBytesToSend() const;

	int					checkNewPartOfRequest(char *partOfRequest);
	int					parseRequest();//парсинг запроса на готовность к обработке(наличие \n\r\n\r) + заполнние полей
	void				prepareResponse();
	void				urlToPath();
	void				response404();
};


#endif //SERVER_REQUESTHANDLER_HPP
