#include "Resource.h"

namespace Crawler {

Resource::Resource( std::string host, std::string name, std::string HTTPresponse ) : name(name) {
	// TODO fazer localName = name - host
	// TODO Obter data a partir do argumento HTTPresponse
	// TODO Procurar em data todas as referencias no mesmo host e adicionar em referencedResources
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
