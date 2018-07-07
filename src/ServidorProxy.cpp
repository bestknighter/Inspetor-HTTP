#include "ServidorProxy.h"

#include <cstdio>
#include <cstdlib>
#include <memory>

#include "Header.h"

ServidorProxy::ServidorProxy( int port ) : continueRunning(true)
										 , requestsReceived(il.requestsReceived)
										 , responsesReceived(el.responsesReceived)
										 , il( port ), el() {}

ServidorProxy::~ServidorProxy() {}

bool ServidorProxy::Loop() {
	/**** Saindo da maquina ****/
	continueRunning &= il.acceptConnections();
	continueRunning &= il.receiveRequests();

	// Autoriza requests
	// (quem deve fazer isso eh a UI, mas esta aqui como placeholder para testes)
	if( requestsReceived.size() > 0 ) std::swap( requestsReceived, requestsToSend );

	// Envia requests
	if( requestsToSend.size() > 0 ) {
		#ifdef DEBUG
		printf( "\nSending %lu requests.\n", (long int) requestsToSend.size() );
		#endif // DEBUG
		for( auto it = requestsToSend.begin(); it != requestsToSend.end(); it++ ) {
			#ifdef DEBUG
			printf( "\nSending to %s:%s...\n%s\n\n", std::get<1>(*it).host.c_str(), std::get<1>(*it).port.c_str(), std::get<1>(*it).to_string(false).c_str() );
			#endif // DEBUG
			if( -1 == el.sendRequest( std::get<0>(*it), std::get<1>(*it) ) ) {
				fprintf( stderr, "\nCouldn't send data. Closing everything. Try again.\n" );
				il.closeSocket( std::get<0>(*it).lock()->getFileDescriptor() ); // Ao fechar o socket interno, no proximo loop os externos tbm serao encerrados
			}
		}
		requestsToSend.clear();
	}

	/**** Chegando na maquina ****/
	continueRunning &= el.receiveResponses();

	// Autoriza requests
	// (quem deve fazer isso eh a UI, mas esta aqui como placeholder para testes)
	if( responsesReceived.size() > 0 ) std::swap( responsesReceived, responsesToSend );

	// Envia requests
	if( responsesToSend.size() > 0 ) {
		#ifdef DEBUG
		printf( "\nSending %lu responses.\n", (long int) responsesToSend.size() );
		#endif // DEBUG
		for( auto it = responsesToSend.begin(); it != responsesToSend.end(); it++ ) {
			#ifdef DEBUG
			printf( "\nSending...\n%s\n\n", std::get<1>(*it).to_string(false).c_str() );
			#endif // DEBUG
			if( -1 == il.sendResponse( std::get<0>(*it), std::get<1>(*it) ) ) {
				fprintf( stderr, "\nCouldn't send data. Closing everything. Try again.\n" );
				il.closeSocket( std::get<0>(*it).lock()->getFileDescriptor() ); // Ao fechar o socket interno, no proximo loop os externos tbm serao encerrados
			}
		}
		responsesToSend.clear();
	}

	return continueRunning;
}
