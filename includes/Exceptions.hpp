#ifndef SERVER_EXCEPTIONS_HPP
# define SERVER_EXCEPTIONS_HPP
# include <iostream>
# include <errno.h>
# include <cstring>

class Exceptions {
private:
	Exceptions();
	Exceptions(const Exceptions &);
	virtual ~Exceptions();
	Exceptions		&operator=(const Exceptions &);
public:
	class SocketCreationException : public std::exception{
	public:
		virtual const char  *what() const throw(){
			return ("SOCKET CREATING ERROR");
		}
	};
	class BindException : public std::exception{
	public:
		virtual const char  *what() const throw(){
			return ("BIND ERROR");
		}
	};
	class SelectException : public std::exception{
	public:
		virtual const char  *what() const throw(){
			return ("SELECT ERROR");
		}
	};
	class AcceptException : public std::exception{
	public:
		virtual const char  *what() const throw(){
			return ("ACCEPT ERROR");
		}
	};
	class FcntlException : public std::exception{
	public:
		virtual const char  *what() const throw(){
			return ("FCNTL ERROR");
		}
	};
	class ReadException : public std::exception{
	public:
		virtual const char  *what() const throw(){
			std::cout << std::strerror(errno) << std::endl;
			return ("READ ERROR");
		}
	};

};


#endif //SERVER_EXCEPTIONS_HPP
