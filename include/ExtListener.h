#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>

#include "MessageData.h"

namespace Socket {

class ExtListener {
public:
	ExtListener();
	~ExtListener();
	
	ssize_t Send( int connectionIDofInternal, std::string addr, int port, std::string message );
	void ReceiveMessages();

	std::vector< MessageData > messagesReceived;
private:
	typedef struct {
		int socket;
		std::string addr;
		int port;
		struct sockaddr_in serv_addr;
		int connectionIDofInternal;
	} SocketInfo;

	std::vector< SocketInfo > connections;
	fd_set active_fd_set;
	fd_set read_fd_set;
	struct timeval timeout;
};

};
