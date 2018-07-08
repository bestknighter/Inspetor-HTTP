#include "Resource.h"

#include "Header.h"

namespace Crawler {

Resource::Resource( std::string host, std::string name, std::string HTTPresponse ) : host(host), name(name) {
	int begin = name.find( host );
	localName = name.replace( begin, host.length()+1, "" );

	HTTP::Header header( HTTPresponse );
	data = header.body;

	// TODO Procurar em data todas as referencias no mesmo host e adicionar em referencedResources
	// Somente se Content-Type for text/html (verificar se so esse mesmo)
}

std::string Resource::getName() {
	return name;
}

std::string Resource::getLocalName() {
	return localName;
}

std::vector< std::string > Resource::getReferencedResources() {
	return referencedResources;
}

bool save( std::string rootPath ) {
	// TODO salvar data no caminho rootPath/name corrigindo as referencias para deles rootPath/name
}

};
