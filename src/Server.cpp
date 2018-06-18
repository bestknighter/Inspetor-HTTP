#include "Server.h"

namespace Socket {

Server::Server( int port ) : port( port ), socket( -1 ), state( 0 ) {
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
		}
	}
}

Server::~Server() {
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

bool Server::Start( int queueSize ) {
	state |= SERVER_STARTED;
	return listen( socket, queueSize ) >= 0;
}

void Server::Run() {
	state |= SERVER_RUNNING;

	read_fd_set = active_fd_set;
	timeout.tv_sec = 0;
	timeout.tv_usec = 250;
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

bool Server::IsValid() {
	return state > 0;
}

bool Server::IsReady() {
	return state & SOCKET_BINDED > 0;
}

bool Server::IsStarted() {
	return state & SERVER_STARTED > 0;
}

bool Server::IsRunning() {
	return state & SERVER_RUNNING > 0;
}

ssize_t Server::Send( int connectionID, std::string message ) {
	if( state & SERVER_RUNNING ) {
		if( connectionID < connections.size() ) {
			return send( connections[connectionID].socket , message.c_str(), message.length(), 0 );
		} else {
			printf( "\nCannot send message. Invalid connectionID.\n" );
		}
	} else {
		printf( "\nCannot send message. Socket is invalid or is not connected.\n" );
	}
	return 0;
}

void Server::RunReceiver( int connectionID ) {
	while( state & SERVER_RUNNING ) {
		if( connectionID < connections.size() ) {
			char buffer[1024];
			int valread = read( connections[connectionID].socket, buffer, 1024 );
			if( state & SERVER_RUNNING ) {
				messagedReceived.emplace_back( std::string( buffer, valread ), connectionID );
			}
		} else {
			printf( "\nCannot receive message. Invalid connectionID.\n" );
		}
	}
}

};