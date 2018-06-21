#include "IntListener.h"

#include <cstdio>

#define INTLISTENER_TIMEOUT_MS 25
#define QUEUESIZE 5

namespace Socket {

IntListener::IntListener( int port ) : port( port ), socket( -1 ), state( 0 ) {
	if( (socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
		printf( "\nSocket creation error\n" );
	} else {
		state |= SOCKET_CREATED;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_LOOPBACK;
		address.sin_port = htons( port );
		if( bind( socket, (struct sockaddr *) &address, sizeof(address) ) < 0 ) {
			printf( "\nFailed to bind socket.\n" );
		} else {
			state |= SOCKET_BINDED;
			FD_ZERO( &active_fd_set );
			FD_SET( socket, &active_fd_set );
			if( listen( socket, queueSize ) < 0 ) {
				printf( "\nFailed to start listening.\n" );
			} else {
				state |= INTLISTENER_STARTED;
			}
		}
	}
}

IntListener::~IntListener() {
	state = SHUTTING_DOWN;
	for( int i = connections.size() - 1; i >= 0; i-- ) {
		close( connections[i].socket );
		FD_CLR( connections[i].socket, &active_fd_set );
	}
	if( socket >= 0 ) {
		close( socket );
		FD_CLR( socket, &active_fd_set );
	}
	state = 0;
}

void IntListener::AcceptAndReceive() {
	state |= INTLISTENER_RUNNING;

	read_fd_set = active_fd_set;
	timeout.tv_sec = 0;
	timeout.tv_usec = INTLISTENER_TIMEOUT_MS;
	if( select( FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout ) < 0 ) {
		printf( "\nCould not wait on select for sockets.\n" );
	}
	for( int i = 0; i < FD_SETSIZE; i++ ) {
		if( FD_ISSET(i, &read_fd_set ) ) {
			if( i == socket ) {
				// Accepting connections
				ConnectionData cd;
				socklen_t length = sizeof(struct sockaddr_in);
				cd.socket = accept( socket, (struct sockaddr*) &(cd.address), &length );
				if( -1 < cd.socket ) {
					connections.push_back( cd );
					FD_SET( cd.socket, &active_fd_set );
				}
			} else {
				// Receiving data
				int connectionID;
				for( connectionID = 0; connectionID < connections.size(); connectionID++ ) {
					if( connections[connectionID].socket == i ) break;
				}
				char buffer[1024];
				int valread = read( i, buffer, 1024 );
				if( 0 < valread) {
					messagesReceived.emplace_back( std::string( buffer, valread ), connectionID );
				} else if( 0 == valread ) {
					close( i );
					FD_CLR( i, &active_fd_set );
					connections.erase( connections.begin() + connectionID );
				}
			}
		}
	}
}

ssize_t IntListener::Send( int connectionID, std::string message ) {
	if( state & INTLISTENER_RUNNING ) {
		if( connectionID < connections.size() ) {
			return send( connections[connectionID].socket , message.c_str(), message.length(), 0 );
		} else {
			printf( "\Invalid connectionID.\n" );
		}
	} else {
		printf( "\nCannot send message. Socket is invalid or is not connected.\n" );
	}
	return 0;
}

};
