#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <vector>
#include <tuple>

#define field std::tuple< std::string, std::string >

namespace HTTP {

struct Header {
	Header( std::string header );

	std::string to_string();

	std::string firstLine;
	std::vector< field > fields;
	std::string body;

	std::string host;
	std::string port;
};

};

#endif // HEADER_H
