#include "Response.hpp"

Response::Response(){}

Response::Response(const Response &other){
	*this = other;
}

Response::~Response() {
}

Response&	Response::operator=( const Response &other){
	if (this != &other) {
		_serverAnswer = other._serverAnswer;
		_headers = other._headers;
		_body = other._body;
	}
	return (*this);
}

void Response::setServerAnswer(const std::string &serverAnswer) {
	_serverAnswer = serverAnswer;
}

void 	Response::setUpBody(std::stringstream &buffer) {
	_body = buffer.str();
	buffer.str("");
	buffer << _body.length();
	_contentLength = buffer.str();
}

void Response::setUpHeaders() {
	_headers += "Server: geniusx/1.1.0\n";
	_headers += "Date: Sun, 20 Jun 2021 08:59:22 GMT\n";//generate time
	_headers += "Content-Type: text/html\n";//generate cont type
	_headers += "Content-Lenght: ";
	_headers += _contentLength;
	_headers += "\n";
	_headers += "\r\n\r\n";
}
const std::string Response::receiveAnswer(){
	return (_serverAnswer + _headers + _body + "\r\n\r\n");
}

