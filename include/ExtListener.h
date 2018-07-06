#ifndef EXTLISTENER_H
#define EXTLISTENER_H

#include <string>
#include <vector>
#include <queue>
#include <memory>

#include <unistd.h>

#include "Socket.h"
#include "Header.h"

class ExtListener {
public:
	ExtListener( int port );
	ExtListener();
	~ExtListener();

	ssize_t sendRequest( std::weak_ptr< Socket > requestingSocket, HTTP::Header request );
	void receiveResponses();

	std::queue< std::tuple< std::weak_ptr< Socket >, HTTP::Header > > responsesReceived; // Socket que deve receber; O que deve receber
private:
	int findSocketPair( std::weak_ptr< Socket > s_w_ptr );
	int findSocketPair( std::shared_ptr< Socket > s_s_ptr );
	void trimSockets();

	std::vector< std::tuple< std::shared_ptr< Socket >, std::weak_ptr< Socket > > createdSockets; // Socket externo; Socket interno
};

#endif // EXTLISTENER_H
