#ifndef INTLISTENER_H
#define INTLISTENER_H

#include <string>
#include <vector>
#include <queue>
#include <memory>

#include <unistd.h>

#include "Socket.h"
#include "Header.h"

class IntListener {
public:
	IntListener( int port );
	~IntListener();
	
	void acceptConnections();
	void receiveRequests();
	void closeSocket( int fileDescriptor );
	ssize_t sendResponse( int fileDescriptor, std::string message );

	std::queue< std::tuple< std::weak_ptr< Socket >, HTTP::Header > > requestsReceived; // Socket que pediu; O que pediu
private:
	Socket listeningSocket;
	std::vector< std::shared_ptr< Socket > > connectedSockets;

};

#endif // INTLISTENER_H
