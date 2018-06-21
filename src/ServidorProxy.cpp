#include "ServidorProxy.h"

ServidorProxy::ServidorProxy( int port ) : il( port ), el() {}

ServidorProxy::~ServidorProxy() {}

bool ServidorProxy::Loop() {
	// Leaving machine first
	il.AcceptAndReceive();
	printf( "\nThere are %d outbound packets.\n", il.messagesReceived.size() );
	for( int i = 0; i < il.messagesReceived.size(); i++ ) {
		printf( "\nMessage from %s:%d to %s:%d\n%s\n"
			, il.messagesReceived[i].addr_from.c_str()
			, il.messagesReceived[i].port_from
			, il.messagesReceived[i].addr_to.c_str()
			, il.messagesReceived[i].port_to
			, il.messagesReceived[i].message.c_str()
		);
		el.Send( il.messagesReceived[i].internalConnectionID, il.messagesReceived[i].addr_to, il.messagesReceived[i].port_to, il.messagesReceived[i].message );
	}

	// Arriving machine last
	el.ReceiveMessages();
	printf( "\nThere are %d inbound packets.\n", el.messagesReceived.size() );
	for( int i = 0; i < el.messagesReceived.size(); i++ ) {
		printf( "\nMessage from %s:%d to %s:%d\n%s\n"
			, il.messagesReceived[i].addr_from.c_str()
			, il.messagesReceived[i].port_from
			, il.messagesReceived[i].addr_to.c_str()
			, il.messagesReceived[i].port_to
			, il.messagesReceived[i].message.c_str()
		);
		il.Send( el.messagesReceived[i].internalConnectionID, el.messagesReceived[i].message );
	}
	return true;
}
