//
// Created by enoelia on 08.06.2021.
//

#include "RequestHandler.hpp"

RequestHandler::RequestHandler(Server *server) : _server(server){
	_response = new Response();
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

int					RequestHandler::checkNewPartOfRequest(char *partOfRequest){
	_rawRequest += partOfRequest;
	if (!parseRequest()){//парсинг запроса на готовность к обработке(наличие \n\r\n\r) + заполнение полей
		return 0;
		//1) ищем \r\n\r\n
		//2) если нашли то контен сайз и сравнить с сайзом configa( ReuestHandler._server->_max_body_size), если ошибка уставint _badContentSize;
		//3) если не нашли контент сайз то это весь запрос вернуть (1) _url = /index.html
		//4) PUT, DELETE, POST, GET указано что то другое ставишь флаг _wrongMethods
		//
	} else {
		prepareResponse();
		_rawRequest = "";
		return 1;
	}
}

int					RequestHandler::parseRequest(){
	//<Заглушка>
	_method = GET;
	_url = "/site";
	_headers.insert( std::pair<std::string, std::string>("Content-Length","555"));
	return (1);
	//</Заглушка>
}

void				RequestHandler::prepareResponse(){
	setUpPathFromUrl(std::string::npos);
	std::ifstream file(_filePath.c_str());
	std::stringstream buffer;

	std::cout << _filePath.c_str() << std::endl;
	//std::cout << buffer.str() << std::endl << std::endl;
	buffer << file.rdbuf();
	if (!file){
		response404();
	} else {
		// если урл не указывает на файл смотерть в индекс, если нет индекса, искать файл индекс.хтмл если файла нет, смотреть autoindex, если и его нет то ошибка 403
		// елси файла нет, вернуть 404
		// если метод не соответвует доступном ошибку 405 и указать досутпные в заголовке Allow
		if (_method == GET) {
			_response->setServerAnswer("HTTP/1.1 200 OK\n");
			_response->setUpBody(buffer);
			_response->setUpHeaders();
		}
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
			_filePath = (*it)->root + _url;
			_currentLocation = *it;
			return (1);
		}
	}
	return (setUpPathFromUrl(newLastSlashUrlPos));
}

void				RequestHandler::response404(){
	std::ifstream file(_server->get404Path().c_str());
	std::stringstream buffer;

	_response->setServerAnswer("HTTP/1.1 404 Not Found\n");
	if (!file) {
		buffer << "<!DOCTYPE html>\n<html><title>404</title><body>Error 404 Not Found</body></html>\n";
		_response->setUpBody(buffer);
		_response->setUpHeaders();
	} else {
		buffer << file.rdbuf();
	}
	//	res += "HTTP/1.1 200 OK\nContent-Length: ";
	//	res += std::to_string(str.length());
	//	res += "\r\n\r\n";
	//	res += str;
	//	//res += "<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n";
	//	res +=  "\r\n\r\n";
	//	std::cout << res << std::endl;
	//	writeValue = write(_socketFd, res.c_str(), res.length());
	//	std::cout << "write return : " <<writeValue << std::endl;
	//	_status = CLOSE;
}
