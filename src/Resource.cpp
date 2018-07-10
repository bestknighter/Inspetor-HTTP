#include "Resource.h"

#include "Header.h"

namespace Crawler {

Resource::Resource( std::string host, std::string name, std::string HTTPresponse ) : host(host) {
	int begin = name.find( host );
	localName = name.replace( begin, host.length()+1, "" );
	this->name = host + "/" + localName;
	if( localName.empty() ) localName = "index";

	HTTP::Header header( HTTPresponse );
	bool isHtml = false;
	for( unsigned int i = 0; i < header.fields.size(); i++ ) {
		if( std::get<0>( header.fields[i] ) == "Content-Type" ) {
			isHtml = std::get<1>( header.fields[i] ).find( "text/html" ) != std::string::npos;
			break;
		}
	}
	if( isHtml && header.firstLine == "HTTP/1.1 200 OK" ) {
		data = header.body;
		searchReferences( "<a href=\"" );
		valid = true;
	} else {
		valid = false;
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

void Resource::setReferences( std::vector< long long int > refs ) {
	for( unsigned int i = 0, j = 0; i < refs.size(); i++, j++ ) {
		if( refs[i] != -1 ) {
		    references.push_back( refs[i] );
		} else {
			referencedResources.erase( referencedResources.begin() + j-- );
		}
	}
}

std::vector< unsigned long int > Resource::getReferences() {
	return references;
}

bool Resource::save( std::string rootPath ) {
	// TODO salvar data no caminho rootPath/name corrigindo as referencias para deles rootPath/name
}

bool Resource::isValid() {
	return valid;
}

/*
root
 ├─ 1st-lvl
 │   ├─ 2nd-level
 │   ├─ 2nd-level
 │   │   └─ 3rd-level
 │   └─ 2nd-level
 ├─ 1st-level
 └─ 1st-level
*/
void Resource::printInTree( unsigned int level, bool last ) {
	unsigned int i = 0;
	for( ; i < level-1; i++ ) {
		printf( " %c  ", 179 ); // " │  "
	}
	if( i < level ) {
		printf( " %c%c ", last?192:195, 196 ); // last ? " └─ " : " ├─ "
	}
	printf( "%s", localName.c_str() );
}

void Resource::searchReferences( const char* HTMLproperty ) {
	std::string property( HTMLproperty );
	unsigned long long int begin = data.find( property );
	while( begin != std::string::npos ) {
		begin += property.length();
		unsigned long long int end = data.find( "\"", begin );
		std::string reference = data.substr(begin, end - begin);
		unsigned long long int foundHost;
		if( reference.size() > 1 && reference[0] == '/' && reference[1] != '/' ) {
			foundHost = 1;
		} else if( (foundHost = reference.find( host ), foundHost != std::string::npos) ) {
			foundHost += host.length() + 1;
		}
		if( foundHost != std::string::npos ) { // So adiciona se for local, e adiciona somente o caminho relativo (depois da primeria "/")
			reference = foundHost <= reference.length() ? reference.substr( foundHost ) : std::string( "" );
			if( reference.find( ".html" ) != std::string::npos || reference.find( "." ) == std::string::npos ) referencedResources.emplace_back( begin, end, reference );
		}
		begin = data.find( property, end + 2 );
	}
}

};
