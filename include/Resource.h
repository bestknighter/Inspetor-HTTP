#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <vector>
#include <tuple>

namespace Crawler {

struct Resource {
public:
	typedef std::tuple< unsigned long long int, unsigned long long int, std::string > Reference;
	Resource( std::string host, std::string name, std::string HTTPresponse );
	
	std::string getName();
	std::string getLocalName();
	std::vector< Reference > getReferencedResources();
	bool save( std::string rootPath );

	std::vector< unsigned long int > references;
private:
	void searchReferences( const char* HTMLproperty );
	std::string host;
	std::string name;
	std::string localName;
	std::string data;
	std::vector< Reference > referencedResources;
};

};

#endif // RESOURCE_H
