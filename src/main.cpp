#include "Cluster.hpp"
#include "ParseConfig.hpp"

int main(int argc, char **argv){
	(void)argc;
	(void)argv;
	int selectResult;
	int i = 0;
	int j = 0;
	ParseConfig start(argv[1]);
    try {
		start.ParseConf();
		Cluster cluster(&start);
		while (1) {
			cluster.resetSockets();
			selectResult = cluster.serversSelect();
			std::cout << "select result :" << selectResult << std::endl;
			if (selectResult == -1){
				if (errno == EINTR){
					;//what's this1?
				} else {
					throw Exceptions::SelectException();
				}
				continue;//what's this2?
			} else if (selectResult == 0) {
				continue;//time out
			}
			cluster.acceptConnections();
			cluster.readFromSockets();
			cluster.writeToSockets();
			//cluster.closeFds();

		}
    } catch (std::exception &e){
        std::cerr << e.what() << std::endl;
    }
	for (int i = 0; i <= start.getPosServ(); i++)  // <--- Этот кусок кода использовать
		delete start.getServInfo()[i];			   // перед завершением всей программы
}