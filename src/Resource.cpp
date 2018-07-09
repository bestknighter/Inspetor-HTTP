#include "Resource.h"

#include "Header.h"

namespace Crawler {

Resource::Resource( std::string host, std::string name, std::string HTTPresponse ) : host(host), name(name) {
	int begin = name.find( host );
	localName = name.replace( begin, host.length()+1, "" );

	HTTP::Header header( HTTPresponse );
	data = header.body;

	// Somente se Content-Type for text/html (verificar se so esse mesmo)
	bool shouldLookUp = false;
	for( unsigned long int i = 0; i < header.fields.size(); i++ ) {
		if( std::get<0>( header.fields[i] ) == "Content-Type" ) {
			int pos = std::get<1>( header.fields[i] ).find( "text/html" );
			shouldLookUp |= pos != std::string::npos;
			break;
		}
	}
	if( shouldLookUp ) {
		searchReferences( "src=\"" );
		searchReferences( "href=\"" );
	}
}

std::string Resource::getName() {
	return name;
}

std::string Resource::getLocalName() {
	return localName;
}

std::vector< Resource::Reference > Resource::getReferencedResources() {
	return referencedResources;
}

bool Resource::save( std::string rootPath ) {
	// TODO salvar data no caminho rootPath/name corrigindo as referencias para deles rootPath/name
}

void Resource::searchReferences( const char* HTMLproperty ) {
	std::string property( HTMLproperty );
	unsigned long long int begin = data.find( property );
	while( begin != std::string::npos ) {
		begin += property.length();
		unsigned long long int end = data.find( "\"", begin ) - 1;
		std::string reference = data.substr(begin, end - begin);
		unsigned long long int foundHost;
		if( reference[0] == '/' ) {
			foundHost = 1;
		} else if( (foundHost = reference.find( host ), foundHost != std::string::npos) ) {
			foundHost += host.length() + 1;
		}
		if( foundHost != std::string::npos ) { // So adiciona se for local, e adiciona somente o caminho relativo (depois da primeria "/")
			referencedResources.emplace_back( begin, end, reference.substr( foundHost ) );
		}
		begin = data.find( property, end + 2 );
	}
}

};
