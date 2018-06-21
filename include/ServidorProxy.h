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
