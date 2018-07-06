#include "IntListener.h"

#include <cstdio>
#include <cstdlib>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "ErrorPrinter.h"

#define INTLISTENER_BUFFER_SIZE 1024
#define QUEUESIZE 5

IntListener::IntListener( int port ) {
	listeningSocket.listen( "127.0.0.1", std::to_string( port ), QUEUESIZE );
}

IntListener::~IntListener() {
	while( connectedSockets.size() > 0 ) {
		connectedSockets.erase( connectedSockets.end() );
	}
}

void IntListener::acceptConnections() {
	struct pollfd fds( listeningSocket.getFileDescriptor(), POLLIN | POLLPRI, 0 );
	int n = poll( &fds, 1, 0 ); // Verifica se listeningSocket tem conexoes em aguardo
	if( n < 0 ) { // n negativo significa erro
		fprintf( stderr, "\nError when polling listeningSocket.\n" );
		pollError();
	} else if ( n > 0 ) { // n positivo significa que tem conexoes em aguardo
		int newSocketFd = accept( listeningSocket.getFileDescriptor(), nullptr, nullptr );
		if( newSocketFd == -1 ) {
			fprintf( stderr, "\nCould not accept new socket.\n" );
			acceptError();
		} else {
			connectedSockets.emplace_back( new Socket(newSocketFd) );
		}
	}
}

void IntListener::receiveRequests() {
	// Lista de pollfds representa cada socket que pode ter atualizacao
	struct pollfd **fds = (struct pollfd**) malloc( sizeof(struct pollfd*)*connectedSockets.size() );
	for( long int i = 0; i < (long int) connectedSockets.size(); i++ ) {
		fds[i] = new struct pollfd( connectedSockets[i]->getFileDescriptor(), POLLIN | POLLPRI, 0 );
	}

	int n = poll( *fds, connectedSockets.size(), 0 ); // Verifica se tem algun socket conectado que tem coisa para ser lida agora
	if( n < 0 ) { // n negativo significa erro
		fprintf( stderr, "\nError when polling connectedSockets.\n" );
		pollError();
	} else if( n > 0 ) { // n positivo significa que tem n sockets com dados prontos para serem lidos
		for( long int i = (long int) connectedSockets.size() - 1; i >= 0; i-- ) {
			if( (POLLIN | POLLPRI) & fds[i].revents ) { // So pra ter certeza que sao os eventos que quero
				int valread = 0;
				std::string message("");
				do {
					char buffer[INTLISTENER_BUFFER_SIZE];
					valread = read( connectedSockets[i]->getFileDescriptor(), buffer, sizeof( buffer ) );
					message += std::string( buffer, valread );
				} while (INTLISTENER_BUFFER_SIZE == valread);
				if( valread < 0 ) { // Erro ao ler socket
					fprintf( stderr, "\nCould not read data.\n" );
					readError();
				} else if( valread == 0 ) { // Fechar socket
					connectedSockets.erase( connectedSockets.begin() + i );
				} else { // Registra mensagem recebida
					requestsReceived.push_back( std::make_tuple( std::weak_ptr(connectedSockets[i]), HTTP::Header(message) ) );
				}
			}
		}
	}

	// Desaloca lista de pollfds
	for( long int i = 0; i < (long int) connectedSockets.size(); i++ ) {
		delete fds[i];
	}
	free(fds);
}

void IntListener::closeSocket( int fileDescriptor ) {
	for( long int i = 0; i < (long int) connectedSockets.size(); i++ ) {
		if( connectedSockets[i]->getFileDescriptor() == fileDescriptor ) {
			connectedSockets.erase( connectedSockets.begin() + i );
			return;
		}
	}
}

ssize_t IntListener::sendResponse( std::weak_ptr< Socket > receivingSocket, std::string message ) {
	ssize_t sent = send( receivingSocket->getFileDescriptor(), message.c_str(), message.length(), 0 );
	if( sent < 0 ) {
		printf( "\nCould not send data.\n" );
		sendError();
	} else {
		return sent;
	}
	return -1;
}
