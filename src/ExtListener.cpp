#include "ExtListener.h"

#include <cstdio>

#include "ErrorPrinter.h"

#define EXTLISTENER_TIMEOUT_MS 25

namespace Socket {

ExtListener::ExtListener() {
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
	si.socket = -1;
	si.addr = addr;
	si.port = port;
	si.connectionIDofInternal = connectionIDofInternal;

	if( (si.socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
		printf( "\nSocket creation error\n" );
		socketError();
	} else {
		memset( &(si.serv_addr), '0', sizeof( si.serv_addr ) );

		si.serv_addr.sin_family = AF_INET;
		si.serv_addr.sin_port = htons( port );

		// Convert IPv4 and IPv6 addresses from text to binary form
		if( inet_pton( AF_INET, addr.c_str(), &(si.serv_addr.sin_addr) ) <= 0 ) {
			printf( "\nInvalid address / Address not supported.\n" );
		} else if( connect( si.socket, (struct sockaddr *) &(si.serv_addr), sizeof(si.serv_addr) ) < 0 ) {
			printf( "\nConnection Failed\n" );
			connectError();
		} else {
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
				md.port_to = 8228;
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
