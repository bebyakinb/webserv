#include "Response.hpp"

Response::Response(){
	_mime_map = {
			{"css", "text/css"},
			{"gif", "image/gif"},
			{"htm", "text/html"},
			{"html", "text/html"},
			{"jpeg", "image/jpeg"},
			{"jpg", "image/jpeg"},
			{"ico", "image/x-icon"},
			{"js", "application/javascript"},
			{"pdf", "application/pdf"},
			{"mp4", "video/mp4"},
			{"png", "image/png"},
			{"svg", "image/svg+xml"},
			{"xml", "text/xml"},
			{ "", "text/plain"}
	};

}

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

void Response::setUpContentType(const std::string &extension) {
	_contentType = _mime_map[extension];
	if (_contentType.empty()){
		_contentType = "text/plain";
	}
}

void			Response::setDate()
{
	char			buffer[100];
	struct timeval	tv;
	struct tm		*tm;

	gettimeofday(&tv, NULL);
	tm = gmtime(&tv.tv_sec);
	strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
	_date = std::string(buffer);
}

void Response::setUpHeaders(const std::string &extension) {
	struct tm current_time;

	setUpContentType(extension);
	setDate();
	_headers += "Server: geniusx/1.1.0\n";
	_headers += "Date: ";
	_headers += _date;
	_headers += "\n";
	_headers += "Content-Type: ";
	_headers += _contentType;
	_headers += "\n";
	_headers += "Content-Lenght: ";
	_headers += _contentLength;
	_headers += "\r\n\r\n";
}

const std::string Response::receiveAnswer(){
	return (_serverAnswer + _headers + _body + "\r\n\r\n");
}

