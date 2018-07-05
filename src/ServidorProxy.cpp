#include "ServidorProxy.h"

#include "Header.h"

ServidorProxy::ServidorProxy( int port ) : il( port ), el( port ) {}

ServidorProxy::~ServidorProxy() {}

bool ServidorProxy::Loop() {
	// Leaving machine first
	il.AcceptAndReceive();
	if(0 < (int) il.messagesReceived.size() ) printf( "\nThere are %d outbound packets.\n", (int) il.messagesReceived.size() );
	for( int i = 0; i < (int) il.messagesReceived.size(); i++ ) {
		HTTP::Header header( il.messagesReceived[i].message );
		printf( "\nMessage from %s:%d to %s:%s\n%s\n"
			, il.messagesReceived[i].addr_from.c_str()
			, il.messagesReceived[i].port_from
			, header.host.c_str()
			, header.port.c_str()
			, header.to_string( false ).c_str()
		);
		el.Send( il.messagesReceived[i].internalConnectionID, header.host.c_str(), std::atoi( header.port.c_str() ), il.messagesReceived[i].message );
	}
	il.messagesReceived.clear();

	// Arriving machine last
	el.ReceiveMessages();
	if(0 < (int) el.messagesReceived.size() ) printf( "\nThere are %d inbound packets.\n", (int) el.messagesReceived.size() );
	for( int i = 0; i < (int) el.messagesReceived.size(); i++ ) {
		HTTP::Header header( el.messagesReceived[i].message );
		printf( "\nMessage from %s:%d to %s:%s\n%s\n"
			, el.messagesReceived[i].addr_from.c_str()
			, el.messagesReceived[i].port_from
			, header.host.c_str()
			, header.port.c_str()
			, header.to_string( false ).c_str()
		);
		il.Send( el.messagesReceived[i].internalConnectionID, el.messagesReceived[i].message );
	}
	el.messagesReceived.clear();
	return true;
}
