#include "ExtListener.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "ErrorPrinter.h"

#define EXTLISTENER_BUFFER_SIZE 1024
#define EXTLISTENER_WAITTIME 25

namespace Inspector {

ExtListener::ExtListener() {}

ExtListener::~ExtListener() {
	while( createdSockets.size() > 0 ) {
		createdSockets.erase( createdSockets.end() );
	}
}

ssize_t ExtListener::sendRequest( std::weak_ptr< Socket > requestingSocket, HTTP::Header request ) {
	SocketPair socketPair;
	int n = findSocketPair(requestingSocket);
	if( -1 == n ) { // Nao encontrou um par
		Socket* s = new Socket();
		if( s->connectTo( request.host, request.port ) ) { // Sucesso ao conectar
			socketPair = std::make_tuple( std::shared_ptr<Socket>(s), requestingSocket );
			createdSockets.push_back( socketPair );
		} else { // Falha ao conectar
			fprintf( stderr, "\nCould not connect to send request.\n" );
			delete s;
			return -1;
		}
	} else { // Achou um par
		socketPair = createdSockets[n];
	}
	// So chega nessa parte se tiver um par, seja criado ou encontrado
	ssize_t sent = send( std::get<0>(socketPair)->getFileDescriptor(), request.to_string().c_str(), request.to_string().length(), 0 );
	if( sent < 0 ) {
		printf( "\nCould not send data." );
		sendError();
	}
	return sent;
}

bool ExtListener::receiveResponses() {
	trimSockets();
	if( createdSockets.size() <= 0 ) return true;
	
	bool retValue = true;
	
	// Lista de pollfds representa cada socket que pode ter atualizacao
	struct pollfd* fds = new struct pollfd[createdSockets.size()];
	for( long int i = 0; i < (long int) createdSockets.size(); i++ ) {
		fds[i].fd = std::get<0>(createdSockets[i])->getFileDescriptor();
		fds[i].events = POLLIN | POLLPRI;
		fds[i].revents = 0;
	}

	for( long int i = (long int) createdSockets.size() - 1; i >= 0; i-- ) {
		int pollRet = poll( fds+i, 1, 0 );
		if( pollRet > 0 && (POLLIN | POLLPRI) & fds[i].revents ) {
			int valread = 0;
			std::string message("");
			do {
				char buffer[EXTLISTENER_BUFFER_SIZE] = {0};
				valread = read( std::get<0>(createdSockets[i])->getFileDescriptor(), buffer, sizeof( buffer ) );
				if( 0 == valread ) break;
				message += std::string( buffer, valread );
				
				int aux = errno;
				pollRet = poll( fds+i, 1, EXTLISTENER_WAITTIME );
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
				createdSockets.erase( createdSockets.begin() + i );
			} else { // Registra mensagem recebida
				responsesReceived.push_back( std::make_tuple( std::get<1>(createdSockets[i]), HTTP::Header(message) ) );
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

int ExtListener::findSocketPair( std::weak_ptr< Socket > s_w_ptr ) {
	trimSockets();
	if( s_w_ptr.expired() ) return -1; // Pra que testar se ja expirou?
	for( long int i = 0; i < (long int) createdSockets.size(); i++ ) {
		std::shared_ptr< Socket > inner( std::get<1>(createdSockets[i]).lock() );
		std::shared_ptr< Socket > outer( s_w_ptr.lock() );
		if( inner.get() == outer.get() ) { // Mesmo endereco
			if( *inner.get() == *outer.get() ) { // Objs identicos
				return i;
			}
		}
	}
	return -1; // Nao encontrou
}

int ExtListener::findSocketPair( std::shared_ptr< Socket > s_s_ptr ) {
	trimSockets();
	if( s_s_ptr.get() == nullptr ) return -1; // Pra que testar se ja expirou?
	for( long int i = 0; i < (long int) createdSockets.size(); i++ ) {
		if( std::get<0>( createdSockets[i] ) == s_s_ptr ) { // Sao identicos
			return i;
		} else {
			std::shared_ptr< Socket > inner( std::get<0>(createdSockets[i]) );
			if( inner.get() == nullptr ) { // Expirou
				// Sockets externos (de possessao de ExtListener) nunca expiram sem serem
				// excluidos, entao esse trecho provavelmente nunca sera executado
				continue;
			}
			if( inner.get() == s_s_ptr.get() ) { // Mesmo endereco
				if( *inner.get() == *s_s_ptr.get() ) { // Objs identicos
					return i;
				}
			}
		}
	}
	return -1; // Nao encontrou
}

void ExtListener::trimSockets() { // Exclui pares cujo o socket interno foi excluido
	for( long int i = (long int) createdSockets.size() - 1; i >= 0 ; i-- ) {
		std::weak_ptr< Socket > s_w_ptr = std::get<1>(createdSockets[i]);
		if( s_w_ptr.expired() ) createdSockets.erase( createdSockets.begin() + i );
	}
}

};
