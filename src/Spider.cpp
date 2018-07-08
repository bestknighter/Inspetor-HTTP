#include "Spider.h"

#include <queue>

#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#define SPIDER_BUFFER 1024
#define SPIDER_WAITTIME 250

namespace Crawler {

Spider::Spider( std::string host ) : success(false), treeRootName(host) {
	Socket socket;
	if( !socket.connectTo(host, "80") ) return;
	std::queue< std::string > resourcesToDownload;
	resourcesToDownload.push( host );

	struct pollfd fd;
	fd.events = POLLIN | POLLPRI;
	fd.revents = 0;
	while( !resourcesToDownload.empty() ) {
		if( findResources( resourcesToDownload.front() ) == -1 ) {
			std::string request( "GET http://" );
			request += resourcesToDownload.front();
			request += " HTTP/1.1\r\nHost: " + host + "\r\nConnection: keep-alive\r\n\r\n";

			// Solicita recurso
			ssize_t sent = send( socket.getFileDescriptor(), request.c_str(), request.length(), 0 );
			if( sent < 0 ) {
				printf( "\nCould not send data." );
				sendError();
			}

			// Recebe recurso
			fd.fd = socket.getFileDescriptor(); // Servidor pode ter me feito reabrir um socket
			int pollRet = poll( &fd, 1, 0 );
			if( pollRet > 0 && (POLLIN | POLLPRI) & fd.revents ) {
				int valread = 0;
				std::string message("");
				do {
					char buffer[SPIDER_BUFFER] = {0};
					valread = read( socket.getFileDescriptor(), buffer, sizeof( buffer ) );
					if( 0 == valread ) break;
					message += std::string( buffer, valread );
					
					int aux = errno;
					pollRet = poll( &fd, 1, SPIDER_WAITTIME );
					if( pollRet < 0 ) {
						fprintf( stderr, "\nError when polling spider socket." );
						pollError();
						return;
					}
					errno = aux;
				} while( pollRet > 0 && (POLLIN | POLLPRI) & fd.revents );
				if( valread < 0 ) { // Erro ao ler socket
					fprintf( stderr, "\nCould not read data." );
					readError();
					return;
				} else if( 0 == valread ) { // Fechar socket
					socket = Socket();
					if( !socket.connectTo( host, "80" ) ) return;
				} else { // Registra mensagem recebida
					tree.emplace_back( host, resourcesToDownload.front(), message );
					// TODO Adicionar referencias na fila
				}
			} else if( pollRet < 0 ) {
				fprintf( stderr, "\nError when polling spider socket." );
				pollError();
				return;
			}
			resourcesToDownload.pop();
		}
	}

	// TODO achar e setar as referencias na arvore
	success = true;
}

bool Spider::isValid() {
	return success;
}

long long int Spider::findResource( std::string resourceName ) {
	for( int i = 0; i < tree.size(); i++ ) {
		if( tree[i].getName() == resourceName ) return i;
	}
	return -1;
}

};
