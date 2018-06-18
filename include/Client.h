#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <string>

namespace Socket {

class Client {
public:
	Client( std::string addr = "127.0.0.1", int port );
	~Client();
	
	bool IsValid();
	bool IsConnected();
	ssize_t Send( std::string message );
	std::string Receive();
private:
	int port;
	std::string addr;
	int socket;
	bool connected;
	
	struct sockaddr_in address;
	struct sockaddr_in serv_addr;
};

};
