#include "Header.h"

namespace HTTP {

Header::Header( std::string header ) {
	if( !header.empty() ) {
		unsigned int first = 0;
		unsigned int last = header.find( "\r\n" );
		firstLine = header.substr( first, last );
		
		first = last + 2;
		while( ( last = header.find( "\r\n", first ), last ) > first ) {
			std::string line = header.substr( first, last );
			unsigned int division = line.find( ":" );
			std::string property = line.substr( 0, division );
			std::string value = line.substr( division + 2 );

			if( "Host" == property ) {
				division = value.rfind( ":" );
				host = value.substr( 0, division );
				if( division < value.size() )
					port = value.substr( division + 1 );
				
			}

			fields.push_back( std::make_tuple( property, value ) );

			first = last + 2;
		}

		if( host.empty() ) {
			unsigned int begin = firstLine.find( "//" ) + 2;
			unsigned int end = firstLine.find( "/", begin );
			std::string value = firstLine.substr( begin, end );
			unsigned int division = value.rfind( ":" );
			host = value.substr( 0, division );
			if( division < value.size() )
				port = value.substr( division + 1 );
		}
		if( '[' == host[0] ) {
			host = host.substr( 1, host.size()-1 );
		}
		if( port.empty() ) {
			port = "80";
		}

		body = header.substr( first + 2 );
	}
}

std::string Header::to_string() {
	std::string request( "" );

	request += firstLine + "\r\n";

	for( unsigned int i = 0; i < fields.size(); i++ ) {
		request += std::get<0>( fields[i] ) + ": " + std::get<1>( fields[i] ) + "\r\n";
	}

	request += "\r\n";
	request += body;

	return request;
}

};
