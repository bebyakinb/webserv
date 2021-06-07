#include "Cluster.hpp"
int main(int argc, char **argv){
    std::vector<ServerConfig> confs;
    ServerConfig servConf;

    confs.push_back(servConf);
    (void *)argv;
    try {
        Cluster cluster(confs);
        //std::cout << AF_INET << std::endl;
		for (std::vector<Server>::const_iterator it = cluster.getServers().begin() ; it != cluster.getServers().end(); ++it){
			std::cout << &(it->getSocketAddr()) << std::endl;
			std::cout << ntohs(it->getSocketAddr().sin_port) << std::endl;
		}
    } catch (std::exception &e){
        std::cerr << e.what() << std::endl;
    }
    std::cout << "ya tut1" << std::endl;
}