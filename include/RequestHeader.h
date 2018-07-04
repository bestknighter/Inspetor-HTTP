#ifndef REQUESTHEADER_H
#define REQUESTHEADER_H

#include <string>
#include <vector>
#include <tuple>

#define field std::tuple< std::string, std::string >

namespace HTTP {

struct RequestHeader {
	RequestHeader();
	RequestHeader( std::string header );

	std::string to_string();

	std::string firstLine;
	std::string body;

	std::vector< field > fields;

	// Standart fields
	std::string A_IM;
	std::string Accept;
	std::string Accept_Charset;
	std::string Accept_Encoding;
	std::string Accept_Language;
	std::string Accept_Datetime;
	std::string Accept_Control_Request_Method;
	std::string Accept_Control_Request_Headers;
	std::string Authorization;
	std::string Cache_Control;
	std::string Connection;
	std::string Content_Length;
	std::string Content_MD5;
	std::string Content_Type;
	std::string Cookie;
	std::string Date;
	std::string Expect;
	std::string Forwarded;
	std::string From;
	std::string Host;
	std::string If_Match;
	std::string If_Modified_Since;
	std::string If_None_Match;
	std::string If_Range;
	std::string If_Unmodified_Since;
	std::string Max_Forwards;
	std::string Origin;
	std::string Pragma;
	std::string Proxy_Authorization;
	std::string Range;
	std::string Referer;
	std::string TE;
	std::string User_Agent;
	std::string Upgrade;
	std::string Via;
	std::string Warning;

	// Non-standart but common
	std::string Upgrade_Insecure_Requests;
	std::string X_Requested_With;
	std::string DNT;
	std::string X_Forwarded_For;
	std::string X_Forwarded_Host;
	std::string X_Forwarded_Proto;
	std::string Front_End_Https;
	std::string X_HTTP_Method_Override;
	std::string X_Att_Deviceid;
	std::string x_wap_profile;
	std::string Proxy_Connection;
	std::string X_UIDH;
	std::string X_Csrf_Token;
	std::string X_Request_ID;
	std::string X_Correlation_ID;
};

};

#endif // REQUESTHEADER_H
