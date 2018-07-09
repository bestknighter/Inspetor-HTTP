#ifndef SPIDER_H
#define SPIDER_H

#include <vector>

#include "Socket.h"
#include "Resource.h"

namespace Crawler {

class Spider {
public:
	Spider( std::string host );
	bool isValid();
private:
	std::string downloadResource( std::string host, std::string resourceName );
	long long int findResource( std::string resourceName );
	bool success;
	std::string treeRootName;
	std::vector< Resource > tree;
};

};

#endif // SPIDER_H
