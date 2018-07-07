#include "IntListener.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "ErrorPrinter.h"

#define INTLISTENER_BUFFER_SIZE 1024
#define INTLISTENER_WAITTIME 25
#define QUEUESIZE 5

IntListener::IntListener( int port ) {
	if( !listeningSocket.listenTo( "127.0.0.1", std::to_string( port ), QUEUESIZE ) ) {
		fprintf( stderr, "\nCould not set up listening socket...\n" );
	}
}

IntListener::~IntListener() {
	while( connectedSockets.size() > 0 ) {
		connectedSockets.erase( connectedSockets.end() );
	}
}

bool IntListener::acceptConnections() {
	if( !listeningSocket.isValid() ) return false;
	
	bool retValue = true;
	
	struct pollfd fds;
	fds.fd = listeningSocket.getFileDescriptor();
	fds.events = POLLIN | POLLPRI;
	fds.revents = 0;
	int n = poll( &fds, 1, 0 ); // Verifica se listeningSocket tem conexoes em aguardo
	if( n < 0 ) { // n negativo significa erro
		fprintf( stderr, "\nError when polling listeningSocket." );
		pollError();
		retValue = false;
	} else if ( n > 0 ) { // n positivo significa que tem conexoes em aguardo
		int newSocketFd = accept( listeningSocket.getFileDescriptor(), nullptr, nullptr );
		if( newSocketFd == -1 ) {
			fprintf( stderr, "\nCould not accept new socket." );
			acceptError();
			retValue = false;
		} else {
			connectedSockets.emplace_back( new Socket(newSocketFd) );
		}
	}
	return retValue;
}

bool IntListener::receiveRequests() {
	if( connectedSockets.size() == 0 ) return true;
	
	bool retValue = true;
	
	// Lista de pollfds representa cada socket que pode ter atualizacao
	struct pollfd* fds = new struct pollfd[connectedSockets.size()];
	for( long int i = 0; i < (long int) connectedSockets.size(); i++ ) {
		fds[i].fd = connectedSockets[i]->getFileDescriptor();
		fds[i].events = POLLIN | POLLPRI;
		fds[i].revents = 0;
	}

	for( long int i = (long int) connectedSockets.size() - 1; i >= 0; i-- ) {
		int pollRet = poll( fds+i, 1, 0 );
		if( pollRet > 0 && (POLLIN | POLLPRI) & fds[i].revents ) {
			int valread = 0;
			std::string message("");
	    	do {
	    		char buffer[INTLISTENER_BUFFER_SIZE] = {0};
				valread = read( connectedSockets[i]->getFileDescriptor(), buffer, sizeof( buffer ) );
				if( 0 == valread ) break;
				message += std::string( buffer, valread );
				
				int aux = errno;
				pollRet = poll( fds+i, 1, INTLISTENER_WAITTIME );
				if( pollRet < 0 ) {
					fprintf( stderr, "\nError when polling external socket %ld.", i );
					pollError();
					retValue = false;
	    		}
	    		errno = aux;
	    	} while( pollRet > 0 && (POLLIN | POLLPRI) & fds[i].revents );
			if( valread < 0 ) { // Erro ao ler socket
				fprintf( stderr, "\nCould not read data." );
				readError();
				retValue = false;
			} else if( 0 == valread ) { // Fechar socket
				// Precisa notificar IntListener que socket externo foi fechado? Acho que nao
				connectedSockets.erase( connectedSockets.begin() + i );
			} else { // Registra mensagem recebida
				requestsReceived.push_back( std::make_tuple( std::weak_ptr< Socket >(connectedSockets[i]), HTTP::Header(message) ) );
			}
	    } else if( pollRet < 0 ) {
			fprintf( stderr, "\nError when polling external socket %ld.", i );
			pollError();
			retValue = false;
	    }
	}

	// Desaloca lista de pollfds
	delete[] fds;
	return retValue;
}

void IntListener::closeSocket( int fileDescriptor ) {
	for( long int i = 0; i < (long int) connectedSockets.size(); i++ ) {
		if( connectedSockets[i]->getFileDescriptor() == fileDescriptor ) {
			connectedSockets.erase( connectedSockets.begin() + i );
			return;
		}
	}
}

ssize_t IntListener::sendResponse( std::weak_ptr< Socket > receivingSocket, HTTP::Header response ) {
    if( receivingSocket.expired() ) {
    	printf( "\nCouldn't send data. Internal connection already closed.\n" );
    	return -1;
    }
	ssize_t sent = send( receivingSocket.lock()->getFileDescriptor(), response.to_string().c_str(), response.to_string().length(), 0 );
	if( sent < 0 ) {
		printf( "\nCould not send data.\n" );
		sendError();
	} else {
		return sent;
	}
	return -1;
}
