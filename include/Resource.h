#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <vector>

namespace Crawler {

struct Resource {
public:
	Resource( std::string host, std::string name, std::string HTTPresponse );
	
	std::string getName();
	std::string getLocalName();
	std::vector< std::string > getReferencedResources();
	bool save( std::string rootPath );

	std::vector< unsigned long int > references;
private:
	std::string host;
	std::string name;
	std::string localName;
	std::string data;
	std::vector< std::string > referencedResources;
};

};

#endif // RESOURCE_H
