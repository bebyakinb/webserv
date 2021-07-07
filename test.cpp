#define READ_ERR 1
#define BUFFER_SIZE 2
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>

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

int main() {
	//int fd = open("server.cpp", O_RDONLY);
	//std::cout << readAll(fd);
	std::map<std::string, std::string> ff;
	ff.insert(std::pair<std::string, std::string>("asa", "sasass"));
	std::cout << ff["asaa"] << std::endl;
}