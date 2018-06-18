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
		}
	}
}

Server::~Server() {
	state = SHUTTING_DOWN;
	while( state != 0 ) {}
	for( int i = connections.size() - 1; i >= 0; i-- ) {
		close( connections[i].socket );
	}
	if( socket >= 0 ) {
		close( socket );
	}
}

bool Server::Start( int queueSize ) {
	state |= SERVER_STARTED;
	return listen( socket, queueSize ) >= 0;
}

void Server::Run() {
	state |= SERVER_RUNNING;
	while( state & SERVER_RUNNING ) {

		// Accepting connections
		ConnectionData cd;
		socklen_t length;
		cd.socket = accept( socket, (struct sockaddr*) &(cd.address), &length );
		if( -1 < cd.socket ) {
			if( sizeof(struct sockaddr_in) != length ) {
				printf( "\nUnexpected socket address type encountered.\n" );
				close( cd.socket );
			} else {
				if( state & SERVER_RUNNING ) {
					connections.push_back( cd );
				} else {
					close( cd.socket );
				}
			}
		}

		// Receiving data
		char buffer[1024];
		for( int connectionID = connections.size(); connectionID >= 0; connectionID-- ) {
			int valread = read( connections[connectionID].socket, buffer, 1024 );
			if( 0 < valread) {
				messagedReceived.emplace_back( std::string( buffer, valread ), connectionID );
			} else if( 0 == valread ) {
				close( connections[connectionID].socket );
				connections.erase( connections.begin() + connectionID );
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