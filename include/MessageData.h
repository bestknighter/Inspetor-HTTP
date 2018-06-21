#ifndef MESSAGEDATA_H
#define MESSAGEDATA_H

#include <string>

namespace Socket {

typedef struct {
	std::string message;
	int internalConnectionID;

	std::string addr_from;
	int port_from;

	std::string addr_to;
	int port_to;
} MessageData;

};

#endif // MESSAGEDATA_H
