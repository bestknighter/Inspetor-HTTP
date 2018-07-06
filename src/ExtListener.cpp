#include "ExtListener.h"

#include <cstdlib>
#include <cstdio>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "ErrorPrinter.h"

#define EXTLISTENER_BUFFER_SIZE 1024

ExtListener::ExtListener() {}

ExtListener::~ExtListener() {
	while( createdSockets.size() > 0 ) {
		createdSockets.erase( createdSockets.end() );
	}
}

ssize_t ExtListener::sendRequest( std::weak_ptr< Socket > requestingSocket, HTTP::Header request ) {
	std::tuple< std::shared_ptr< Socket >, std::weak_ptr< Socket > socketPair;
	int n = findSocketPair(requestingSocket);
	if( -1 == n ) { // Nao encontrou um par
		Socket* s = new Socket();
		if( s->connect( request.host, request.port ) ) { // Sucesso ao conectar
			socketPair = std::make_tuple( std::shared_ptr(s), requestingSocket );
			createdSockets.push_back( socketPair );
		} else { // Falha ao conectar
			fprintf( stderr, "\nCould not connect to send request.\n" );
			delete s;
			return -1;
		}
	} else { // Achou um par
		socketPair = createdSocket[n];
	}
	// So chega nessa parte se tiver um par, seja criado ou encontrado
	ssize_t sent = send( std::get<0>(socketPair)->getFileDescriptor(), request.to_string().c_str(), request.to_string().length(), 0 );
	if( sent < 0 ) {
		printf( "\nCould not send data.\n" );
		sendError();
	}
	return sent;
}

void ExtListener::receiveResponses() {
	trimSockets();
	// Lista de pollfds representa cada socket que pode ter atualizacao
	struct pollfd **fds = (struct pollfd**) malloc( sizeof(struct pollfd*)*createdSockets.size() );
	for( int i = 0; i < createdSockets.size(); i++ ) {
		fds[i] = new struct pollfd( std::get<0>(createdSockets[i])->getFileDescriptor(), POLLIN | POLLPRI, 0 );
	}

	int n = poll( *fds, createdSockets.size(), 0 ); // Verifica se tem algun socket criado que tem coisa para ser lida agora
	if( n < 0 ) { // n negativo significa erro
		fprintf( stderr, "\nError when polling createdSockets.\n" );
		pollError();
	} else if( n > 0 ) { // n positivo significa que tem n sockets com dados prontos para serem lidos
		for( int i = createdSockets.size() - 1; i <= 0; i-- ) {
			if( (POLLIN | POLLPRI) & fds[i].revents ) { // So pra ter certeza que sao os eventos que quero
				int valread = 0;
				std::string message("");
				do {
					char buffer[EXTLISTENER_BUFFER_SIZE];
					valread = read( std::get<0>(createdSockets[i])->getFileDescriptor(), buffer, sizeof( buffer ) );
					message += std::string( buffer, valread );
				} while (EXTLISTENER_BUFFER_SIZE == valread);
				if( valread < 0 ) { // Erro ao ler socket
					fprintf( stderr, "\nCould not read data.\n" );
					readError();
				} else if( valread == 0 ) { // Fechar socket
					// Precisa notificar IntListener que socket externo foi fechado? Acho que nao
					createdSockets.erase( createdSockets.begin() + i );
				} else { // Registra mensagem recebida
					requestsReceived.push_back( std::make_tuple( std::weak_ptr(createdSockets[i]), HTTP::Header(message) ) );
				}
			}
		}
	}

	// Desaloca lista de pollfds
	for( int i = 0; i < connectedSockets.size(); i++ ) {
		delete fds[i];
	}
	free(fds);
}

int ExtListener::findSocketPair( std::weak_ptr< Socket > s_w_ptr ) {
	trimSockets();
	if( s_w_ptr.expired() ) return -1; // Pra que testar se ja expirou?
	for( int i = 0; i < createdSockets.size(); i++ ) {
		if( std::get<1>( createdSockets[i] ) == s_w_ptr ) { // Sao identicos
			return i;
		} else {
			std::shared_ptr inner( std::get<1>(createdSockets[i]).lock() );
			std::shared_ptr outer( s_w_ptr.lock() );
			if( inner.get() == outer.get() ) { // Mesmo endereco
				if( *inner.get() == *outer.get() ) { // Objs identicos
					return i;
				}
			}
		}
	}
	return -1; // Nao encontrou
}

int ExtListener::findSocketPair( std::shared_ptr< Socket > s_s_ptr ) {
	trimSockets();
	if( s_s_ptr.get() == nullptr ) return -1; // Pra que testar se ja expirou?
	for( int i = 0; i < createdSockets.size(); i++ ) {
		if( std::get<0>( createdSockets[i] ) == s_s_ptr ) { // Sao identicos
			return i;
		} else {
			std::shared_ptr inner( std::get<0>(createdSockets[i]) );
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
	for( int i = createdSockets.size() - 1; i <= 0 ; i-- ) {
		std::weak_ptr< Socket > s_w_ptr = std::get<1>(createdSockets[i]);
		if( s_w_ptr.expired() ) createdSockets.erase( createdSockets.begin() + i );
	}
}
