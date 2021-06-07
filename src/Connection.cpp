#include "Connection.hpp"

Connection::Connection(int listenSocketFd) : _status(READ), _addrlen(sizeof(_addr)){
	int flags;

	_socketFd = accept(listenSocketFd,(sockaddr*)&_addr, &_addrlen);
	if (_socketFd == -1){
		throw Errors::AcceptException();
	}
	std::cout << "SUCCESS CONNECTION FROM IP " <<  inet_ntoa(_addr.sin_addr) << std::endl;//
	std::cout << "CREATE NEW FD = " << _socketFd << std::endl;

	//nonblocking mode
//	if ((flags = fcntl(_socketFd, F_GETFL)) == -1)
//		throw Errors::FcntlException();
//	if ((fcntl(_socketFd, F_SETFL, flags | O_NONBLOCK) == -1))
//		throw Errors::FcntlException();
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

	if ((readValue = read(_socketFd, buf, BUFFER_SIZE + 1)) == -1) {
		throw Errors::ReadException();
	}
	if (readValue > 0){
		_request += buf;
		if (parseRequest())
			_status = WRITE;
	}else{
		_status = CLOSE;
	}
	std::cout << "read return : " <<readValue << " BUFF = " << BUFFER_SIZE << std::endl;
}

void		Connection::writeToSocket(){
	int					writeValue;
	std::ifstream		t("file.txt");
	std::stringstream	buffer;

	buffer << t.rdbuf();
	writeValue = write(_socketFd, buffer.str().c_str(), 78);//buffer.str().length());
	std::cout << "write return : " <<writeValue << std::endl;
	_status = CLOSE;
}

int			Connection::parseRequest(){
	_request.clear();
	return 1;
}