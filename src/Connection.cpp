#include "Connection.hpp"

Connection::Connection(int listenSocketFd, Server *server) :
	_status(READ),
	_addrlen(sizeof(_addr)),
	_requestHandler(new RequestHandler(server)),
	_alreadySent(0){
	int flags;

	_socketFd = accept(listenSocketFd,(sockaddr*)&_addr, &_addrlen);
	if (_socketFd == -1){
		throw Exceptions::AcceptException();
	}
	std::cout << "SUCCESS CONNECTION FROM IP " <<  inet_ntoa(_addr.sin_addr) << std::endl;//
	std::cout << "CREATE NEW FD = " << _socketFd << std::endl;

	//nonblocking mode
	if ((flags = fcntl(_socketFd, F_GETFL)) == -1)
		throw Exceptions::FcntlException();
	if ((fcntl(_socketFd, F_SETFL, flags | O_NONBLOCK) == -1))
		throw Exceptions::FcntlException();
}

Connection::Connection(const Connection &other){
	*this = other;
}

Connection::~Connection() {
}

Connection&	Connection::operator=( const Connection &other){
	if (this != &other) {
		_status = other._status;
		_socketFd = other._socketFd;
		_addr = other._addr;
		_addrlen = other._addrlen;
	}
	return (*this);
}

int			Connection::getStatus() const {
	return _status;
}

void		Connection::setStatus(int status) {
	_status = status;
}

int			Connection::getSocketFd() const {
	return _socketFd;
}

void		Connection::setSocketFd(int fd) {
	_socketFd = fd;
}

void		Connection::readFromSocket() {
	int readValue;
	char buf[BUFFER_SIZE];

	std::cout << "try read from socket " << _socketFd << std::endl;
	if ((readValue = read(_socketFd, buf, BUFFER_SIZE + 1)) == -1) {
		throw Exceptions::ReadException();
	}
	if (readValue > 0){
		if (_requestHandler->checkNewPartOfRequest(buf)){
			_status = WRITE;
		}
		//std::cout << _requestHandler->getRawRequest() << std::endl;//
	} else {
		close(_socketFd);
		std::cout << "\n close fd:" << _socketFd << std::endl;
		_status = CLOSE;
	}
	//std::cout << "read return : " << readValue << " BUFF = " << BUFFER_SIZE << std::endl;
}

void		Connection::writeToSocket(){
	std::string tmp;

	tmp = _requestHandler->getAnswer();
	_alreadySent += write(_socketFd, tmp.c_str() + _alreadySent, tmp.length() - _alreadySent);
	//std::cout << tmp << std::endl;
	//std::cout << _alreadySent << "//" << tmp.length() << std::endl;

	if (_alreadySent == _requestHandler->getBytesToSend()){
		close(_socketFd);
		std::cout << "\n close fd:" << _socketFd << std::endl;
		_status = CLOSE;
	}
	   // stream used for the conversion
//	std::ifstream t("/home/enoelia/01_21school/webserv/www/index.html");
//	std::string str((std::istreambuf_iterator<char>(t)),
//					std::istreambuf_iterator<char>());
//	std::ostringstream convert;
//
//	std::string res = "";
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
