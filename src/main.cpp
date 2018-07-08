#include <cstdio>
#include <cstdlib>
#include <string>
#include <stdexcept>

#include "ServidorProxy.h"

#define DEFAULT_PORT 8228

int main( int argc, char* argv[] ) {
	
	int port = DEFAULT_PORT;
	if( 3 == argc ) {
		if( std::string( argv[1] ) == "-p" ) {
			try {
				port = std::stoi( std::string( argv[2] ) );
			} catch( std::exception e ) {
				printf( "\nInvalid port.\n" );
				return -1;
			}
		} else {
			printf( "\nOption not recognized.\nValid options:\n[-p <port>]\tchooses different port other than the default %d\n", port );
			return -1;
		}
	} else if( 1 != argc ) {
		printf( "\nInvalid argument numbers.\n" );
		return -1;
	}

	printf( "\nStarting aracne with port# %d\n", port );

	Inspector::ServidorProxy s( port );

	while( s.Loop() ) {};

	return 0;
}
