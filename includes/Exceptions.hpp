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
	class WrongArgsNumException : public std::exception{
	public:
		virtual const char  *what() const throw(){
			return ("WRONG ARGUMENTS NUMBER");
		}
	};
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
	class ReadException : public std::exception {
	public:
		virtual const char *what() const throw() {
			std::cout << std::strerror(errno) << std::endl;
			return ("READ ERROR");
		}
	};
	class ServerNameException : public std::exception {
	public:
		virtual const char *what() const throw() {
			std::cout << std::strerror(errno) << std::endl;
			return ("SERVER NAME IN CONFIG IS EMPTY");
		}
	};
	class HostException : public std::exception {
	public:
		virtual const char *what() const throw() {
			std::cout << std::strerror(errno) << std::endl;
			return ("HOST IN CONFIG IS EMPTY");
		}
	};
	class PortException : public std::exception {
	public:
		virtual const char *what() const throw() {
			std::cout << std::strerror(errno) << std::endl;
			return ("PORT IN CONFIG IS WRONG");
		}
	};
	class MaxBodySizeException : public std::exception {
	public:
		virtual const char *what() const throw() {
			std::cout << std::strerror(errno) << std::endl;
			return ("MAX BODY SIZE IN CONFIG IS WRONG");
		}
	};
	class MethodsException : public std::exception {
	public:
		virtual const char *what() const throw() {
			std::cout << std::strerror(errno) << std::endl;
			return ("ONE OF THE METHODS IN CONFIG IS WRONG");
		}
	};
	class NoSuchErrorException : public std::exception {
	public:
		virtual const char *what() const throw() {
			std::cout << std::strerror(errno) << std::endl;
			return ("No Such Error");
		}
	};
};


#endif //SERVER_EXCEPTIONS_HPP
