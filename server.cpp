#define BUFFER_SIZE 65536
#define SOCK_CREATE_ERR 1
#define BIND_ERR 2
#define SELECT_ERR 3
#define ACCEPT_ERR 4
#define FCNTL_ERR 5
#define READ_ERR 6

#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <iostream>
#include <cstring>

typedef struct t_socket{
	int					fd;
	std::string			readBuf;
	std::string			writeBuf;
} s_socket;

void exitError(int err_num){
	if (err_num == SOCK_CREATE_ERR){
		std::cerr << "SOCKET CREATING ERROR" << std::endl;
	} else if (err_num == BIND_ERR){
		std::cerr << "BIND ERROR" << std::endl;
	} else if (err_num == SELECT_ERR){
		std::cerr << "SELECT ERROR" << std::endl;
	} else if (err_num == ACCEPT_ERR){
		std::cerr << "ACCEPT ERROR" << std::endl;
	} else if (err_num == FCNTL_ERR){
		std::cerr << "FCNTL ERROR" << std::endl;
	} else if (err_num == READ_ERR){
		std::cerr << "READ ERROR" << std::endl;
	}
	_exit(1);
}

int craeteSocket(int port){
	int socketFd;
	struct sockaddr_in socketAddr;

	socketAddr.sin_family = AF_INET; 
	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("192.168.20.38");
	socketAddr.sin_port = htons(port);
	
	if ((socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){//AF_INET -- для взаимодействия по сети
	//if ((socketFd = socket(AF_UNIX, SOCK_STREAM, IPPROTO_TCP)) == -1){//AF_UNIX -- для одной машины
		exitError(SOCK_CREATE_ERR);
	}
	if (bind(socketFd, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) == -1){
		exitError(BIND_ERR);
	}
	return (socketFd);
}

int acceptConection(int listenSocketFd){
	int				socketFd;
	sockaddr_in addr;
	socklen_t		addrlen;
	addrlen = sizeof(addr);

	socketFd = accept(listenSocketFd, (sockaddr*)&addr, &addrlen);
	if (socketFd == -1){
		exitError(ACCEPT_ERR);
	}
	std::cout << "SUCCSES CONNECTION FROM IP " <<  inet_ntoa(addr.sin_addr) << std::endl;//
	std::cout << "CREATE NEW FD = " << socketFd << std::endl;
	return (socketFd);
}

void	nonBlockMode(int socketFd){
	int flags;

	if ((flags = fcntl(socketFd, F_GETFL)) == -1)
		exitError(FCNTL_ERR);
	if ((fcntl(socketFd, F_SETFL, flags | O_NONBLOCK) == -1))
		exitError(FCNTL_ERR);
}

void	clearSocket(s_socket socket){
	socket.fd = -1;
	socket.readBuf.clear();
	socket.writeBuf.clear();
}

int		readFromSocket(int fd){
	int			readen;
	char		buf[BUFFER_SIZE];

	if ((readen = read(fd, buf, BUFFER_SIZE)) == -1){
		std::cout << std::strerror(errno) << '\n';
		exitError(READ_ERR);
	}
	std::cout << buf << std::endl;
	//std::cout << result << std::endl;
	return readen;
}

void	createListenSockets(std::vector<int> &listenSocketFds, std::vector<int> &portNums){
	for (std::vector<int>::const_iterator it = portNums.begin() ; it != portNums.end(); ++it){
			listenSocketFds.push_back(craeteSocket(*it));
		}
}

void	resetFdSets(fd_set *readFds, fd_set *writeFds){
	FD_ZERO(readFds);
	FD_ZERO(writeFds);
}

void	setListenSockets(fd_set *readFds, int *maxFd, std::vector<int> &listenSocketFds){
	for (std::vector<int>::const_iterator it = listenSocketFds.begin() ; it != listenSocketFds.end(); ++it){
		*it > *maxFd? *maxFd = *it : 1;
		FD_SET(*it, readFds);
		listen(*it, 16);
	}
}

void	setClientSockets(fd_set *readFds, fd_set *writeFds, int *maxFd, std::vector<s_socket> &clientSockets){
	for (std::vector<s_socket>::const_iterator it = clientSockets.begin() ; it != clientSockets.end(); ++it){
		FD_SET(it->fd, readFds);
		std::cout << it->fd << " added to read" << std::endl;
		if (it->writeBuf.length() > 0){
			FD_SET(it->fd, writeFds);
		}
		if (it->fd > *maxFd){
			*maxFd = it->fd;
		}
	}
}
void	acceptingConections(fd_set *readFds, std::vector<int> &listenSocketFds, std::vector<s_socket> &clientSockets){
	s_socket				tempSocket;

	for (std::vector<int>::const_iterator it = listenSocketFds.begin() ; it != listenSocketFds.end(); ++it){
		if (FD_ISSET(*it, readFds)){
			tempSocket.fd = acceptConection(*it);
			std::cout << "here1" << std::endl;
			nonBlockMode(tempSocket.fd);
			clientSockets.push_back(tempSocket);
			clearSocket(tempSocket);
			std::cout << "here2" << std::endl;
		}
	}
}

int main(void){
	struct timeval			timeout;
	int						select_res;
	//int						newScoketFd;
	std::vector<int>		listenSocketFds;
	std::vector<int>		portNums;
	int						maxFd;
	std::vector<s_socket>	clientSockets;

	fd_set					readFds, writeFds;
	//char						buf[BUFFER_SIZE];

	maxFd = 0;
	portNums.push_back(7674);//
	portNums.push_back(7675);//
	portNums.push_back(7676);//
	createListenSockets(listenSocketFds, portNums);
	while (1){
		std::cout << "here3.1" << std::endl;
		resetFdSets(&readFds, &writeFds);
		setListenSockets(&readFds, &maxFd, listenSocketFds);
		setClientSockets(&readFds, &writeFds, &maxFd, clientSockets);
		
		timeout.tv_sec = 5;
		timeout.tv_usec = 30000;
		select_res = select(maxFd + 1, &readFds, &writeFds, NULL, &timeout);

		if (select_res == -1){
			if (errno == EINTR){
				;//what's this1?
			} else {
				exitError(SELECT_ERR);
			}
			continue;//what's this2?
		} else if (select_res == 0) {
			continue;//time out
		}
		acceptingConections(&readFds, listenSocketFds, clientSockets);
		for (std::vector<s_socket>::const_iterator it = clientSockets.begin() ; it != clientSockets.end(); ++it){
			std::cout << it->fd << " searched in fdlist" << std::endl;
			if (FD_ISSET(it->fd, &readFds)){
				std::cout << it->fd << " found in fdlist" << std::endl;
				if (!readFromSocket(it->fd)){
					clientSockets.erase(it);
				}
			}
			if (FD_ISSET(it->fd, &writeFds)){
				;
			}
		}
		std::cout << "here3.2" << std::endl;
	}
	return (0);
}
