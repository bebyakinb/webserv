#define READ_ERR 1
#define BUFFER_SIZE 2
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

void exitError(int err_num){
	if (err_num == READ_ERR){
		std::cerr << "SOCKET CREATING ERROR" << std::endl;
	}
}

std::string readAll(int fd){
	int			readen;
	char		buf[BUFFER_SIZE];
	std::string	result;
	
	readen = 1;
	while(readen){
		if ((readen = read(fd, buf, BUFFER_SIZE)) == -1){
			exitError(READ_ERR);
		}
		result += buf;
	}
	return result;
}

int main(){
	int fd = open("server.cpp", O_RDONLY);
	std::cout << readAll(fd);
}