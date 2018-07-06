#ifndef SERVIDORPROXY_H
#define SERVIDORPROXY_H

#include <vector>

#include "ExtListener.h"
#include "IntListener.h"
#include "Header.h"

class ServidorProxy {
public:
	ServidorProxy( int port );
	~ServidorProxy();
	bool Loop();
	
	bool continueRunning;
	std::vector< IntListener::Request > &requestsReceived;
	std::vector< IntListener::Request > requestsToSend;
	std::vector< ExtListener::Response > &responsesReceived;
	std::vector< ExtListener::Response > responsesToSend;
private:
	IntListener il;
	ExtListener el;
};

#endif // SERVIDORPROXY_H
