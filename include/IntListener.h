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

class IntListener {
public:
	IntListener( int port );
	~IntListener();
	
	void AcceptAndReceive();
	ssize_t Send( int connectionID, std::string message );

	std::vector< MessageData > messagesReceived;
private:
	typedef struct {
		int socket;
		struct sockaddr_in address;
	} ConnectionData;

	typedef enum intListenerState : short {
		NEWBORN = 0,
		SOCKET_CREATED = 1 << 0,
		SOCKET_BINDED = 1 << 1,
		INTLISTENER_STARTED = 1 << 2,
		INTLISTENER_RUNNING = 1 << 3,
		SHUTTING_DOWN = 1 << 4
	} IntListenerState;

	int port;
	int socketfd;
	short state;
	std::vector< ConnectionData > connections;
	fd_set active_fd_set;
	fd_set read_fd_set;
	struct timeval timeout;
	
	struct sockaddr_in address;
};

};
