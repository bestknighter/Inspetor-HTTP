#include "Client.h"

namespace Socket {

Client::Client( std::string addr, int port ) : port( port ), addr( addr ), socket( -1 ), connected( false ) {

	if( (socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
		printf( "\nSocket creation error\n" );
	} else {
		memset( &serv_addr, '0', sizeof( serv_addr ) );

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons( port );

		// Convert IPv4 and IPv6 addresses from text to binary form
		if( inet_pton( AF_INET, address.c_str(), &serv_addr.sin_addr ) <= 0 ) {
			printf( "\nInvalid address / Address not supported.\n" );
			connected = false;
		} else {
			connected = connect( socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) >= 0;
			if( !connected ) {
				printf( "\nConnection Failed\n" );
			}
		}
	}

}

Client::~Client() {
	if( socket >= 0 ) {
		close( socket );
	}
}

bool Client::IsValid() {
	return socket >= 0;
}

bool Client::IsConnected() {
	return connected;
}

ssize_t Client::Send( stdd:string message ) {
	if( connected ) {
		return send( socket, message.c_str(), message.length(), 0 );
	} else {
		printf( "\nCannot send message. Socket is invalid or is not connected.\n" );
		return 0;
	}
}

std::string Client::Receive() {
	char buffer[1024];
	int valread = read( socket, buffer, 1024 );
	return std::string( buffer, valread );
}

};