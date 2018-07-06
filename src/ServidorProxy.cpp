#include "ServidorProxy.h"

#include <cstdio>
#include <cstdlib>
#include <memory>

#include "Header.h"

ServidorProxy::ServidorProxy( int port ) : continueRunning(true), il( port ), el() {
	requestsReceived = il.requestsReceived;
	responsesReceived = el.responsesReceived;
}

ServidorProxy::~ServidorProxy() {}

bool ServidorProxy::Loop() {
	/**** Saindo da maquina ****/
	il.acceptConnections();
	il.receiveRequests();

	// Autoriza requests
	// (quem deve fazer isso eh a UI, mas esta aqui como placeholder para testes)
	if( requestsReceived.size() > 0 ) std::vector::swap( requestsReceived, requestsToSend );

	// Envia requests
	if( requestsToSend.size() > 0 ) {
		#ifdef DEBUG
		printf( "\nSending %u requests.\n", requestsToSend.size() );
		#endif // DEBUG
		for( auto it = requestsToSend.begin(); i != requestsToSend.end(); it++ ) {
			#ifdef DEBUG
			printf( "\nSending to %s:%s...\n%s\n\n", std::get<1>(it).host.c_str(), std::get<1>(it).port.c_str(), std::get<1>(it).to_string(false).c_str() );
			#endif // DEBUG
			if( -1 == el.sendRequest( std::get<0>(it), std::get<1>(it) ) ) {
				fprintf( stderr, "\nCouldn't send data. Closing everything. Try again.\n" );
				il.closeSocket( std::get<0>(it)->getFileDescriptor() ); // Ao fechar o socket interno, no proximo loop os externos tbm serao encerrados
			}
		}
		requestsToSend.clear();
	}

	/**** Chegando na maquina ****/
	el.receiveResponses();

	// Autoriza requests
	// (quem deve fazer isso eh a UI, mas esta aqui como placeholder para testes)
	if( responsesReceived.size() > 0 ) std::swap( responsesReceived, responsesToSend );

	// Envia requests
	if( responsesToSend.size() > 0 ) {
		#ifdef DEBUG
		printf( "\nSending %u responses.\n", responsesToSend.size() );
		#endif // DEBUG
		for( auto it = responsesToSend.begin(); i != responsesToSend.end(); it++ ) {
			#ifdef DEBUG
			printf( "\nSending to %s:%s...\n%s\n\n", std::get<1>(it).host.c_str(), std::get<1>(it).port.c_str(), std::get<1>(it).to_string(false).c_str() );
			#endif // DEBUG
			if( -1 == il.sendRequest( std::get<0>(it), std::get<1>(it) ) ) {
				fprintf( stderr, "\nCouldn't send data. Closing everything. Try again.\n" );
				il.closeSocket( std::get<0>(it)->getFileDescriptor() ); // Ao fechar o socket interno, no proximo loop os externos tbm serao encerrados
			}
		}
		responsesToSend.clear();
	}

	return continueRunning;
}
