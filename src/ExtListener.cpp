#include "ExtListener.h"

#include <cstdio>

#define EXTLISTENER_TIMEOUT_MS 25

namespace Socket {

ExtListener::ExtListener() {
	FD_ZERO( &active_fd_set );
}

ExtListener::~ExtListener() {
	for( int i = connections.size() - 1; i >= 0; i-- ) {
		close( connections[i].socket );
		FD_CLR( connections[i].socket, &active_fd_set );
	}
}

ssize_t ExtListener::Send( int connectionIDofInternal, std::string addr, int port, std::string message ) {
	SocketInfo si;
	si.socket = -1;
	si.addr = addr;
	si.port = port;
	si.connectionIDofInternal = connectionIDofInternal;

	if( (si.socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
		printf( "\nSocket creation error\n" );
	} else {
		memset( &(si.serv_addr), '0', sizeof( serv_addr ) );

		si.serv_addr.sin_family = AF_INET;
		si.serv_addr.sin_port = htons( port );

		// Convert IPv4 and IPv6 addresses from text to binary form
		if( inet_pton( AF_INET, addr.c_str(), &(si.serv_addr.sin_addr) ) <= 0 ) {
			printf( "\nInvalid address / Address not supported.\n" );
		} else if( connect( si.socket, (struct sockaddr *) &(si.serv_addr), sizeof(serv_addr) ) < 0 ) {
			printf( "\nConnection Failed\n" );
		} else {
			connections.push_back( si );
			FD_SET( si.socket, &active_fd_set );
			return send( si.socket, message.c_str(), message.length(), 0 );
		}
	}
	return 0;
}

void ExtListener::ReceiveMessages() {
	read_fd_set = active_fd_set;
	timeout.tv_sec = 0;
	timeout.tv_usec = INTLISTENER_TIMEOUT_MS;

	if( select( FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout ) < 0 ) {
		printf( "\nCould not wait on select for sockets.\n" );
	}
	for( int i = 0; i < FD_SETSIZE; i++ ) {
		if( FD_ISSET(i, &read_fd_set ) ) {
			// Receiving data
			int connectionID;
			for( connectionID = 0; connectionID < connections.size(); connectionID++ ) {
				if( connections[connectionID].socket == i ) break;
			}
			char buffer[1024];
			int valread = read( i, buffer, 1024 );
			if( 0 < valread) {
				MessageData md;
				md.message = std::string( buffer, valread );
				md.connectionIDofInternal = connections[connectionID].connectionIDofInternal;
				md.addr_from = connections[connectionID].addr ) );
				md.port_from = connections[connectionID].port;
				md.addr_to = "127.0.0.1";
				md.port_to = port;
				messagesReceived.push_back( md );
			} else if( 0 == valread ) {
				close( i );
				FD_CLR( i, &active_fd_set );
				connections.erase( connections.begin() + connectionID );
			}
		}
	}
}

};
