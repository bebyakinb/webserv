#include "Cluster.hpp"
#include "Parser.hpp"
int main(int argc, char **argv){
	int selectResult;
    try {
		Cluster cluster(Parser(argv[1]).getServerConfigs());
		while (1) {
			cluster.resetSockets();
			selectResult = cluster.serversSelect();
			std::cout << "select result :" << selectResult << std::endl;
			if (selectResult == -1){
				if (errno == EINTR){
					;//what's this1?
				} else {
					throw Errors::SelectException();
				}
				continue;//what's this2?
			} else if (selectResult == 0) {
				continue;//time out
			}
			cluster.acceptConnections();
			cluster.readFromSockets();
			cluster.writeToSockets();
		}

    } catch (std::exception &e){
        std::cerr << e.what() << std::endl;
    }
}