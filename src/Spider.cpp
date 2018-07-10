#include "Spider.h"

#include <queue>
#include <cstdio>

#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "ErrorPrinter.h"
#include "Resource.h"

#define SPIDER_BUFFER 1024
#define SPIDER_WAITTIME 250

namespace Crawler {

Spider::Spider( std::string host ) : success(false), treeRootName(host) {
	if( !socket.connectTo(host, "80") ) return;
	std::queue< std::string > resourcesToDownload;
	resourcesToDownload.push( host );
	short count = 0;

	printf( "Going to %s and computing its tree", host.c_str() );
	fflush( stdout );

	while( !resourcesToDownload.empty() ) {
		count = (count+1) % 10;
		if( 0 == count ) {
			printf( "." );
			fflush( stdout );
		}
		if( findResource( resourcesToDownload.front() ) == -1 ) {
			std::string data( downloadResource( host, resourcesToDownload.front() ) );
			if( data.empty() ) return;
			Resource r( host, resourcesToDownload.front(), data );
			if( r.isValid() ) tree.push_back( r );
			
			// Adicionar referencias na fila
			std::vector< Resource::Reference > refs = tree.back().getReferencedResources();
			for( unsigned int i = 0; i < refs.size(); i++ ) {
				std::string nextResourceName = host + "/" + std::get<2>( refs[i] );
				if( findResource( nextResourceName ) == -1 ) resourcesToDownload.push( nextResourceName );
			}

		}
		resourcesToDownload.pop();
	}

	// Acha e seta as referencias na arvore
	for( unsigned int j = 0; j < tree.size(); j++ ) {
		std::vector< Resource::Reference > refs = tree[j].getReferencedResources();
		std::vector< long long int > foundRefs;
		for( unsigned int i = 0; i < refs.size(); i++ ) {
			foundRefs.push_back( findResource( host + "/" + std::get<2>( refs[i] ) ) );
		}
		tree[j].setReferences( foundRefs );
	}
	printf( "\n" );
	success = true;
}

bool Spider::isValid() {
	return success;
}

std::string Spider::downloadResource( std::string host, std::string resourceName ) {
	int tries = -1;
RETRY:
	tries++;
	std::string request( "GET http://" );
	request += resourceName;
	request += " HTTP/1.1\r\nHost: " + host + "\r\nConnection: keep-alive\r\n\r\n";

	// Solicita recurso
	ssize_t sent = send( socket.getFileDescriptor(), request.c_str(), request.length(), 0 );
	if( sent < 0 ) {
		printf( "\nCould not send data." );
		sendError();
	}

	// Recebe recurso
	struct pollfd fd;
	fd.fd = socket.getFileDescriptor(); // Servidor pode ter me feito reabrir o socket
	fd.events = POLLIN | POLLPRI;
	fd.revents = 0;
	std::string message("");
	int pollRet = poll( &fd, 1, SPIDER_WAITTIME*240 );
	if( pollRet > 0 && (POLLIN | POLLPRI) & fd.revents ) {
		int valread = 0;
		do { // TODO criar Header com primeiro pacote recebido e decidir se deve continuar lendo pelo Content-Type
			 // Somente considerar text/html, text/css e application/javascript
			char buffer[SPIDER_BUFFER] = {0};
			valread = read( socket.getFileDescriptor(), buffer, sizeof( buffer ) );
			if( 0 == valread ) break;
			message += std::string( buffer, valread );
			
			int aux = errno;
			pollRet = poll( &fd, 1, SPIDER_WAITTIME );
			if( pollRet < 0 ) {
				fprintf( stderr, "\nError when polling spider socket." );
				pollError();
				return std::string("");
			}
			errno = aux;
		} while( pollRet > 0 && (POLLIN | POLLPRI) & fd.revents );
		if( valread < 0 ) { // Erro ao ler socket
			fprintf( stderr, "\nCould not read data." );
			readError();
			return std::string("");
		} else if( 0 == valread ) { // Fechar socket
			socket = Socket();
			if( !socket.connectTo( host, "80" ) ) return std::string("");
			if( tries < 3 ) goto RETRY;
		}
	} else if( pollRet < 0 ) {
		fprintf( stderr, "\nError when polling spider socket." );
		pollError();
		return std::string("");
	}
	return message;
}

long long int Spider::findResource( std::string resourceName ) {
	for( unsigned long int i = 0; i < tree.size(); i++ ) {
		if( tree[i].getName() == resourceName ) return i;
	}
	return -1;
}

};
