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
	void setReferences( std::vector< long long int > refs );
	std::vector< unsigned long int > getReferences();
	bool save( std::string rootPath );
	bool isValid();
	void printInTree( unsigned int level, bool last );

private:
	void searchReferences( const char* HTMLproperty );
	std::string host;
	std::string name;
	std::string localName;
	std::string data;
	std::vector< Reference > referencedResources;
	std::vector< unsigned long int > references;
	bool valid;
};

};

#endif // RESOURCE_H

