#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <string>
#include <vector>

namespace Socket {

class Server {
public:
	typedef struct messageData {
		std::string message;
		int connectionID;
	} MessageData;

	Server( int port );
	~Server();
	
	bool Start( int queueSize = 5 );
	void Run();
	bool IsValid();
	bool IsReady();
	bool IsStarted();
	bool IsRunning();
	ssize_t Send( int connectionID, std::string message );

	std::vector< MessageData > messagesReceived;
private:
	int port;
	int socket;
	short state;
	std::vector< ConnectionData > connections;
	
	struct sockaddr_in address;

	typedef struct connectionData {
		int socket;
		struct sockaddr_in address;
	} ConnectionData;

	typedef enum serverState : short {
		NEWBORN = 0,
		SOCKET_CREATED = 1 << 0,
		SOCKET_BINDED = 1 << 1,
		SERVER_STARTED = 1 << 2,
		SERVER_RUNNING = 1 << 3,
		SHUTTING_DOWN = 1 << 4
	} ServerState;
};

};
