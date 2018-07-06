#include "Socket.h"

#include <cstring>

#include "ErrorPrinter.h"

Socket::Socket( int fileDescriptor ) : valid(false), fileDescriptor(fileDescriptor) {
	socklen_t addr_len = sizeof( addr );
	if( -1 == getsockname( fileDescriptor, &addr, &addr_len ) || addr_len != sizeof(addr) ) {
		fprintf( stderr, "\nError on getsockname.\n" );
		getsocknameError();
	} else {
		valid = true;
	}
}

Socket::Socket() : valid(false), fileDescriptor(-1), addr(0, 0, {0}) {}

Socket::~Socket() {
	if( valid ) {
		if( -1 == close( fileDescriptor ) ) {
			closeError();
		} else {
			valid = false;
			fileDescriptor = -1;
		}
	}
}

bool Socket::connect( std::string name, std::string port, int socketFamily, int socketType, int protocol ) {
	struct addrinfo *firstAddr, *addr_p;
	
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = socketFamily;
	hints.ai_socktype = socketType;
	hints.ai_flags = 0;
	hints.ai_protocol = protocol;

	int s = getaddrinfo( name.c_str(), std::to_string( port ).c_str(), &hints, &firstAddr );
	if( s < 0 ) {
		fprintf( stderr, "\nError when looking up name.\n" );
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( s ) );
	} else {
		for( addr_p = firstAddr; addr_p != NULL; addr_p = addr_p->ai_next ) {
			fileDescriptor = socket(addr_p->ai_family, addr_p->ai_socktype, addr_p->ai_protocol);
			if( -1 == fileDescriptor ) {
				#ifdef DEBUG
				fprintf( stderr, "\nFailed creating socket. Trying another address...\n" );
				socketError();
				#endif // DEBUG
				continue;
			}
			if( -1 == connect( fileDescriptor, addr_p->ai_addr, addr_p->ai_addrlen ) ) {
				#ifdef DEBUG
				fprintf( stderr, "\nFailed connecting socket. Trying another address...\n" );
				connectError();
				#endif // DEBUG
				close( fileDescriptor );
			} else {
				valid = true;
				std::memcpy( &(addr), addr_p->ai_addr, addr_p->ai_addrlen );
				break;
			}
		}
		if( NULL == addr_p ) {
			fprintf( stderr, "\nCould not connect to %s:%s...\n", name.c_str(), port.c_str() );
		}
	}
	freeaddrinfo(firstAddr);
	return valid;
}

bool Socket::listen( std::string name, std::string port, int queueSize, int socketFamily, int socketType, int protocol ) {
	struct addrinfo *firstAddr, *addr_p;
	
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = socketFamily;
	hints.ai_socktype = socketType;
	hints.ai_flags = 0;
	hints.ai_protocol = protocol;

	int s = getaddrinfo( name.c_str(), std::to_string( port ).c_str(), &hints, &firstAddr );
	if( s < 0 ) {
		fprintf( stderr, "\nError when looking up name.\n" );
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( s ) );
	} else {
		for( addr_p = firstAddr; addr_p != NULL; addr_p = addr_p->ai_next ) {
			fileDescriptor = socket(addr_p->ai_family, addr_p->ai_socktype, addr_p->ai_protocol);
			if( -1 == fileDescriptor ) {
				#ifdef DEBUG
				fprintf( stderr, "\nFailed creating socket. Trying another address...\n" );
				socketError();
				#endif // DEBUG
				continue;
			}
			if( -1 == bind( fileDescriptor, addr_p->ai_addr, addr_p->ai_addrlen ) ) {
				#ifdef DEBUG
				fprintf( stderr, "\nFailed binding socket. Trying another address...\n" );
				bindError();
				#endif // DEBUG
				close( fileDescriptor );
				continue;
			}
			if( -1 == listen( fileDescriptor, queueSize ) ) {
				fprintf( stderr, "\nFailed to start listening socket. Trying another address...\n" );
				listenError();
				close( fileDescriptor );
			} else {
				valid = true;
				std::memcpy( &(addr), addr_p->ai_addr, addr_p->ai_addrlen );
				break;
			}
		}
		if( NULL == addr_p ) {
			fprintf( stderr, "\nCould not connect to %s:%s...\n", name.c_str(), port.c_str() );
		}
	}
	freeaddrinfo(firstAddr);
	return valid;
}

bool Socket::isValid() {
	return valid;
}

int Socket::getFileDescriptor() {
	return fileDescriptor;
}

unsigned int Socket::getFamily() {
	return addr.sin_family;
}

uint16_t Socket::getPort() {
	return addr.sin_port;
}

uint32_t Socket::getAddress() {
	return addr.sin_addr.s_addr;
}

std::string Socket::getPort() {
	return std::string( ntohs( addr.sin_port ) );
}

std::string Socket::getAddress() {
	return std::string( inet_ntoa( addr.sin_addr ) );
}
