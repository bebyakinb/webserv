#ifndef SERVER_REQUESTHANDLER_HPP
# define SERVER_REQUESTHANDLER_HPP
# include <iostream>
# include <fstream>
# include <map>
# include <sys/stat.h>
# include <climits>
# include <regex>
# include "Server.hpp"
# include "Response.hpp"
# include "utils.hpp"
# include "Cgi.hpp"
# include <list>
# include <dirent.h>

class Server;
class Response;
typedef struct s_location t_location;



class RequestHandler {
private:
	std::string							_rawRequest;
	Server*								_server;

	int									_method;
	std::string							_body;
	std::string							_url;
	std::map<std::string,std::string>	_headers;
	std::map<int,std::string>			_servAnswer;
	std::map<int,std::string>			_defaultErrors;
	
	std::string							_filePath;
	std::string							_filePathLastPart;
	struct s_location					*_currentLocation;
	Response							*_response;
	std::string							_answer;
	unsigned long						_bytesToSend;

	std::vector < std::vector <std::string> >	_strsBody;

	int									_flagParsed;
	int									_badContentSize;
	int 								_flagChuked;
	int									_wrongMethods;	 // 405
	int									_wrongHTTPVersion; // 505
	int									_badRequest;	   // 400

public:
	RequestHandler(Server *server);
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

	int					checkNewPartOfRequest(char *partOfRequest, int size);
	int					parseRequest();//парсинг запроса на готовность к обработке(наличие \n\r\n\r) + заполнние полей
	int					checksAfterParse(std::cmatch result, std::regex rex);
	int					checkDoubleFields(std::cmatch result);
	int					checkHeaders(std::cmatch result, std::regex rex);
	int					checkFirstStr(std::cmatch result, std::regex rex);
	void				prepareResponse();
	void				urlToPath();
	void				responseError(int errNum);
	void				responseToGetRequest();
	int 				setUpPathFromUrl(size_t lastSlashUrlPos);

	void				testPrint(); //удалить потом
	//Kate's methods
	void				responseToPostRequest();
	void				cgi_handler();
	//void				responseToPutRequest();
	void				responseToDeleteRequest();
	void 				pushBody(std::string strBody);
	void				autoindex_execution();
	void				responseAll(std::string first_str, std::string body, const std::string &extension);

};
#endif //SERVER_REQUESTHANDLER_HPP
