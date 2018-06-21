#ifndef SERVIDORPROXY_H
#define SERVIDORPROXY_H

#include <cstdio>
#include <cstdlib>

#include "ExtListener.h"
#include "IntListener.h"

class ServidorProxy {
public:
	ServidorProxy( int port );
	~ServidorProxy();
	bool Loop();
private:
	ExtListener el;
	IntListener il;
};

#endif // SERVIDORPROXY_H
