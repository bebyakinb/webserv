//
// Created by enoelia on 08.06.2021.
//

#include "RequestHandler.hpp"

RequestHandler::RequestHandler(Server *server) : _server(server){
	_response = new Response();
	_method = 0;
	_flagChuked = 0;
	// _flagParsed = 0;
	_wrongMethods = 0;
	_badContentSize = 0;
	_wrongHTTPVersion = 0;
	_badRequest = 0;
	_servAnswer = {
			{ERR400, "HTTP/1.1 400 Bad Request\n"},
			{ERR403, "HTTP/1.1 403 Forbidden\n"},
			{ERR404, "HTTP/1.1 404 Not Found\n"},
			{ERR405, "HTTP/1.1 405 Method Not Allowed\n"},
			{ERR408, "HTTP/1.1 408 Request Timeout\n"},
			{ERR505, "HTTP/1.1 505 HTTP Version Not Supported\n"}
			};
	_defaultErrors = {
			{ERR400,"<!DOCTYPE html>\n<html><title>400</title><body>Error 400 Bad Request</body></html>\n"},
			{ERR403,"<!DOCTYPE html>\n<html><title>403</title><body>Error 403 Forbidden</body></html>\n"},
			{ERR404,"<!DOCTYPE html>\n<html><title>404</title><body>Error 404 Not Found</body></html>\n"},
			{ERR405,"<!DOCTYPE html>\n<html><title>405</title><body>Error 405 Method Not Allowed</body></html>\n"},
			{ERR408,"<!DOCTYPE html>\n<html><title>408</title><body>Error 408 Request Timeout</body></html>\n"},
			{ERR505,"<!DOCTYPE html>\n<html><title>505</title><body>Error 505 HTTP Version Not Supported</body></html>\n"}
	};
}

RequestHandler::RequestHandler(const RequestHandler &other){
	*this = other;
}

RequestHandler::~RequestHandler() {
	delete _response;
}

RequestHandler&	RequestHandler::operator=( const RequestHandler &other){
	if (this != &other) {
		;
	}
	return (*this);
}

int					RequestHandler::getMethod() const {
	return _method;
}

void				RequestHandler::setMethod(int method) {
	_method = method;
}

Server*				RequestHandler::getServer() const {
	return _server;
}

void				RequestHandler::setServer(Server *server) {
	_server = server;
}

const std::string&	RequestHandler::getRawRequest() const {
	return _rawRequest;
}

void				RequestHandler::setRawRequest(const std::string &rawRequest) {
	_rawRequest = rawRequest;
}

const std::string &RequestHandler::getAnswer() const {
	return _answer;
}

unsigned long RequestHandler::getBytesToSend() const {
	return _bytesToSend;
}


void				RequestHandler::testPrint()
{
	std::cout << std::endl;

	std::cout << "_wrongMethods: "<< _wrongMethods << std::endl;
	std::cout << "_badContentSize: " << _badContentSize << std::endl;
	std::cout << "_wrongHTTPVersion: " << _wrongHTTPVersion << std::endl;
	std::cout << "_badRequest: " << _badRequest << std::endl;

	std::cout << std::endl;

	std::cout << "_method: " << _method << std::endl;
	std::cout << "_url: " << _url << std::endl;

	std::cout << std::endl;

	std::cout << "Host: " << _headers["Host"] << std::endl;
	std::cout << "Content-Type: " << _headers["Content-Type"] << std::endl;
	std::cout << "Content-Length: " << _headers["Content-Length"] << std::endl;
	std::cout << "Transfer-Encoding: " << _headers["Transfer-Encoding"] << std::endl;
	std::cout << "test: " << _headers["lol"] << std::endl;

	std::cout << std::endl;

	std::cout << "_body: " << std::endl << std::endl << _body << std::endl;
}

int					RequestHandler::checkNewPartOfRequest(char *partOfRequest, int size){
	_rawRequest.append(partOfRequest, size);
	int status = parseRequest();
	if (status == 0){
		return 0;
	} else if (status > 0){
		prepareResponse();
		_rawRequest = "";
		return 1;
	} else if (status < 0) {
		if (_badContentSize || _badRequest) {
			responseError(ERR400);
		} else if (_wrongMethods) {
			responseError(ERR405);
		} else if (_wrongHTTPVersion) {
			responseError(ERR505);
		} else {
			responseError(ERR400);
		}
	}
}

int					RequestHandler::checkFirstStr(std::cmatch result, std::regex rex)
{
	std::string first_str;

	if (_rawRequest.find("\r\n") != std::string::npos)
	{
		first_str = _rawRequest.substr(0, _rawRequest.find("\r\n") + 2);
		if (std::regex_match(first_str.c_str(), result, rex))
		{
			if (result[1] == "GET")
				_method = 0;
			else if (result[1] == "POST")
				_method = 1;
			else if (result[1] == "DELETE")
				_method = 2;
			else if (result[1] == "PUT")
				_method = 3;
			else
			{
				_wrongMethods = 1;
				return (-1);
			}
			_url = result[2];
			if (result[3] != "HTTP/1.1")
			{
				_wrongHTTPVersion = 1;
				return (-1);
			}
			_flagParsed = 1;
		}
		else if (!first_str.empty())
		{
			_badRequest = 1;
			return (-1);
		}
	}
	return (0);
}


int					RequestHandler::checkDoubleFields(std::cmatch result)
{
	char *tmp = (char *)result[1].str().c_str();

	if (islower(tmp[0]))
		tmp[0] = tmp[0] - 32;
	for (long i = 1; i < result[1].length(); i++)
	{
		if (!islower(tmp[i]))
			tmp[i] = tmp[i] + 32;
	}
	if (_headers.find(tmp) != _headers.end())
		return (-1);
	return (0);
}

int					RequestHandler::checkHeaders(std::cmatch result, std::regex rex)
{
	std::string oneHeader;
	std::string rawRequestTmp = _rawRequest.substr(0, _rawRequest.find("\r\n\r\n") + 4);

	rawRequestTmp.erase(0, rawRequestTmp.find("\r\n") + 2);
	while (rawRequestTmp != "\r\n")
	{
		oneHeader = rawRequestTmp.substr(0, rawRequestTmp.find("\r\n") + 2);
		if (!std::regex_match(oneHeader.c_str(), result, rex) || checkDoubleFields(result))
		{
			_badRequest = 1;
			return (-1);
		}
		_headers[result[1]] = result[2];
		rawRequestTmp.erase(0, rawRequestTmp.find("\r\n") + 2);
	}
	return (0);
}

int					RequestHandler::checksAfterParse()
{
	if (_headers.find("Content-Length") != _headers.end() && _method != 1)
	{
		if ((strtol(_headers["Content-Length"].c_str(), NULL, 10) > _server->getMaxBodySize() ||
			 strtol(_headers["Content-Length"].c_str(), NULL, 10) == 0L ||
			 strtol(_headers["Content-Length"].c_str(), NULL, 10) == LONG_MAX ||
			 strtol(_headers["Content-Length"].c_str(), NULL, 10) < 0) &&
			_headers["Content-Length"] != "0")
		{
			_badContentSize = 1;
			return (-1);
		}
		if (_rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4).size() <
				size_t(std::atoi(_headers["Content-Length"].c_str())))
		{
			_body = _rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4);
			return (0);
		}
		else
			_body = _rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4, std::atoi(_headers["Content-Length"].c_str()));
	}
	if (_headers.find("Content-Length") == _headers.end() &&
		_headers.find("Transfer-Encoding") == _headers.end() &&
		!_rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4).empty())
	{
		_badRequest = 1;
		return (-1);
	}
	return (1);
}

int					RequestHandler::parseRequest()
{
	std::cmatch result;
	std::regex rex_first_str("(^[\r\n]+|[A-Z]+)(?: )"
							 "([\\w\\:\\.\\/-]+)"
							 "(?: )"
							 "(HTTP/+[\\d]+\\.[\\d]+)"
							 "(?:\r\n)");

	std::regex rex_headers("([\\w-]+)"
						   "(?:\\:)"
						   "(?:[ ]{1}|)"
						   "([\\w\\d\\:\\.\\/-]+)"
						   "(?:\r\n)");

	std::regex rex_body("([\\w\\d\\:\\.\\/-]+)"
						"(?:\n\r)");

	for (; _rawRequest[0] == '\r' && _rawRequest[1] == '\n';)
		_rawRequest.erase(0, 2);
	if (_rawRequest.find("\r\n\r\n") != std::string::npos)
	{
		if (checkFirstStr(result, rex_first_str) == -1)
			return (-1);
		if (checkHeaders(result, rex_headers) == -1)
			return (-1);
		return (checksAfterParse());
	}
	return (0);
}


void				RequestHandler::prepareResponse(){
	if (_url.back() != '/' && _url.find('.') == std::string::npos)
		_url += "/";
	setUpPathFromUrl(std::string::npos);
	struct stat buff;
	if (_method == GET && _currentLocation->methods[GET] ) {
		if (stat(_filePath.c_str(), &buff) == -1){
			responseError(ERR404);//нет такого пути или файла
		} else if (S_ISDIR(buff.st_mode)){
			if (!_currentLocation->index.empty()){
				_filePath += _currentLocation->index;
				if (stat((_filePath).c_str(), &buff) == -1) {
					responseError(ERR404);
				} else {
					responseToGetRequest();
				}
			} else if (stat((_filePath + "index.html").c_str(), &buff) != -1) {
				responseToGetRequest();
			} else
				responseError(ERR404);
		} else if (S_ISREG(buff.st_mode)){
			responseToGetRequest();
		}
	} else {
		responseError(ERR405);
	}
	_answer = _response->receiveAnswer();
	_bytesToSend = _answer.length();
}
int 				RequestHandler::setUpPathFromUrl(size_t lastSlashUrlPos){
	size_t newLastSlashUrlPos;
	if ((newLastSlashUrlPos = _url.find_last_of('/', lastSlashUrlPos - 1)) == std::string::npos){
		return (0);
	}
	//какие бывают кейсы?
	// url = "/" location = "/" - OK
	// url = "/index.html" location = "/' - OK
	// url = "www/index.html" location = "/www"
	// url = "www/index.html" location = "/www/"
	// url = "/www/" location = "/www/"
	// URL либо должен указывать на файл либо оканчиваться "/"!!!
	for (std::vector<t_location*>::iterator it = _server->getLocations().begin(); it != _server->getLocations().end(); ++it) {
		std::cout <<"sub str + 1" << _url.substr(0, newLastSlashUrlPos + 1) << std::endl;
		std::cout <<"sub str" << _url.substr(0, newLastSlashUrlPos) << std::endl;
		std::cout <<"url" << (*it)->url << std::endl;
		if (((*it)->url == _url.substr(0, newLastSlashUrlPos + 1)) || ((*it)->url == _url.substr(0, newLastSlashUrlPos))){
			_filePath = (*it)->root + _url.substr(newLastSlashUrlPos);
			_currentLocation = *it;
			return (1);
		}
	}
	return (setUpPathFromUrl(newLastSlashUrlPos));
}

void				RequestHandler::responseError(int errNum){
	std::stringstream buffer;
	std::ifstream file(_server->getErrorPaths()[errNum].c_str());
	std::string serverAnswer = _servAnswer[errNum];
	if (serverAnswer.empty())
		throw Exceptions::NoSuchErrorException();
	if (_server->getErrorPaths()[errNum].empty() || !file ) {
		buffer << _defaultErrors[errNum];
	} else {
		buffer << file.rdbuf();
	}
	_response->setUpBody(buffer);
	_response->setUpHeaders("html");
	_response->setServerAnswer(serverAnswer);
}

void	RequestHandler::responseToGetRequest(){
	std::stringstream buffer;
	std::ifstream file(_filePath.c_str());

	if (!file) {
		responseError(ERR404);
	} else {
		buffer << file.rdbuf();
		_response->setServerAnswer("HTTP/1.1 200 OK\n");
		_response->setUpBody(buffer);
		_response->setUpHeaders(_filePath.substr(_filePath.find_last_of('.') + 1));
	}
}
