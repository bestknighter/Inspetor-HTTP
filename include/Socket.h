#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <string>

class Socket {
public:
	Socket();
	Socket( int fileDescriptor );
	~Socket();

	bool connectTo( std::string name, std::string port, int socketFamily = AF_INET, int socketType = SOCK_STREAM, int protocol = 0 );
	bool listenTo( std::string name, std::string port, int queueSize = 1, int socketFamily = AF_INET, int socketType = SOCK_STREAM, int protocol = 0 );

	bool isValid();
	int getFileDescriptor();
	unsigned int getFamily();
	uint16_t getPort();
	uint32_t getAddress();
	std::string getPortStr();
	std::string getAddressStr();
	
	bool operator==( Socket &rhs );
private:
	bool valid;
	int fileDescriptor;
	struct sockaddr_in addr;
};

#endif // SOCKET_H
