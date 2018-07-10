#ifndef SPIDER_H
#define SPIDER_H

#include <vector>
#include <set>

#include "Socket.h"
#include "Resource.h"

namespace Crawler {

class Spider {
public:
	Spider( std::string host );
	bool isValid();
	void printTree();
private:
	void recursivePrintTree( unsigned long int node, int level, bool last, std::set< unsigned long int >& printedValues );
	std::string downloadResource( std::string host, std::string resourceName );
	long long int findResource( std::string resourceName );
	
	Socket socket;
	bool success;
	std::string treeRootName;
	std::vector< Resource > tree;
};

};

#endif // SPIDER_H
