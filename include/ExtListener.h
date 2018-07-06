#ifndef EXTLISTENER_H
#define EXTLISTENER_H

#include <string>
#include <vector>
#include <memory>

#include <unistd.h>

#include "Socket.h"
#include "Header.h"

class ExtListener {
public:
	typedef std::tuple< std::weak_ptr< Socket >, HTTP::Header > Response; // Socket que deve receber; O que deve receber
	ExtListener( int port );
	ExtListener();
	~ExtListener();

	ssize_t sendRequest( std::weak_ptr< Socket > requestingSocket, HTTP::Header request );
	void receiveResponses();

	std::vector< Response > responsesReceived;
private:
	typedef std::tuple< std::shared_ptr< Socket >, std::weak_ptr< Socket > > SocketPair; // Socket externo (meu); Socket interno (do IntListener)
	int findSocketPair( std::weak_ptr< Socket > s_w_ptr );
	int findSocketPair( std::shared_ptr< Socket > s_s_ptr );
	void trimSockets();

	std::vector< SocketPair > createdSockets;
};

#endif // EXTLISTENER_H
