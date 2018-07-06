#ifndef INTLISTENER_H
#define INTLISTENER_H

#include <string>
#include <vector>
#include <memory>

#include <unistd.h>

#include "Socket.h"
#include "Header.h"

class IntListener {
public:
	typedef std::tuple< std::weak_ptr< Socket >, HTTP::Header > Request; // Socket que pediu; O que pediu
	IntListener( int port );
	~IntListener();
	
	void acceptConnections();
	void receiveRequests();
	void closeSocket( int fileDescriptor );
	ssize_t sendResponse( std::weak_ptr< Socket > receivingSocket, HTTP::Header response );

	std::vector< Request > requestsReceived;
private:
	Socket listeningSocket;
	std::vector< std::shared_ptr< Socket > > connectedSockets;

};

#endif // INTLISTENER_H
