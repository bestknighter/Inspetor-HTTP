#include "ExtListener.h"

#include <cstdio>
#include <netdb.h>
#include <sys/types.h>

#include "ErrorPrinter.h"

#define EXTLISTENER_TIMEOUT_MS 25

namespace Socket {

ExtListener::ExtListener( int port ) : port( port ) {
    errno = 0;
	FD_ZERO( &active_fd_set );
}

ExtListener::~ExtListener() {
	for( int i = connections.size() - 1; i >= 0; i-- ) {
		if( close( connections[i].socket ) < 0 ) {
			closeError();
		}
		FD_CLR( connections[i].socket, &active_fd_set );
	}
}

ssize_t ExtListener::Send( int connectionIDofInternal, std::string addr, int port, std::string message ) {
	SocketInfo si;
	struct addrinfo *ai, *aip;
	
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0; /* Any protocol */

	if( (int s = getaddrinfo( addr.c_str(), htons( port ), &hints, &ai ), 0 > s) ) {
		printf( "\nError when looking up name.\n" );
		printf( "getaddrinfo: %s\n", gai_strerror( s ) );
		return -1;
	}

	for (aip = ai; aip != NULL; aip = aip->ai_next) {
		si.socket = socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
		if( -1 == si.socket ) {
			printf( "\nFailed creating socket. Trying another address...\n" );
			continue;
		}
		if( 0 > connect( si.socket, aip->ai_addr, aip->ai_addrlen ) ) {
			printf( "\nFailed connecting socket. Trying another address...\n" );
			close( si.socket );
		} else {
			break;
		}
	}

	if( NULL == aip ) {
		printf( "\nCould not connect...\n");
		freeaddrinfo(ai);
	} else {
		std::memcpy( &(si.serv_addr), aip->ai_addr, api->ai_addrlen );
		si.addr = std::string( inet_ntoa( si.serv_addr.sin_addr ) );
		si.port = ntohs( si.serv_addr.sin_port );
		si.connectionIDofInternal = connectionIDofInternal;
		freeaddrinfo(ai);

		connections.push_back( si );
		FD_SET( si.socket, &active_fd_set );
		ssize_t sent = send( si.socket, message.c_str(), message.length(), 0 );
		if( sent < -1 ) {
			printf( "\nCould not send data.\n" );
			sendError();
		} else {
			return sent;
		}
	}
	return -1;
}

void ExtListener::ReceiveMessages() {
	read_fd_set = active_fd_set;
	timeout.tv_sec = 0;
	timeout.tv_usec = EXTLISTENER_TIMEOUT_MS;

	if( select( FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout ) < 0 ) {
		printf( "\nCould not wait on select for sockets.\n" );
		selectError();
	}
	for( int i = 0; i < FD_SETSIZE; i++ ) {
		if( FD_ISSET(i, &read_fd_set ) ) {
			// Receiving data
			int connectionID;
			for( connectionID = 0; connectionID < (int) connections.size(); connectionID++ ) {
				if( connections[connectionID].socket == i ) break;
			}
			int valread = 0;
			std::string message("");
			do {
				char buffer[1024];
				valread = read( i, buffer, sizeof( buffer ) );
				message += std::string( buffer, valread );
			} while (1024 == valread);
			if( 0 < valread) {
				MessageData md;
				md.message = message;
				md.internalConnectionID = connections[connectionID].connectionIDofInternal;
				md.addr_from = connections[connectionID].addr;
				md.port_from = connections[connectionID].port;
				md.addr_to = "127.0.0.1";
				md.port_to = port;
				messagesReceived.push_back( md );
			} else if( 0 == valread ) {
				if( close( i ) < 0 ) {
					closeError();
				}
				FD_CLR( i, &active_fd_set );
				connections.erase( connections.begin() + connectionID );
			} else {
				printf( "\nCould not read data.\n" );
				readError();
			}
		}
	}
}

};
