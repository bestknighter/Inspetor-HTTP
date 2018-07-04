#include "RequestHeader.h"

namespace HTTP {

RequestHeader::RequestHeader() {}

RequestHeader::RequestHeader( std::string header ) {
	if( !header.empty() ) {
		unsigned int first = 0;
		unsigned int last = header.find( "\r\n" );
		firstLine = header.substr( first, last );
		
		first = last + 2;
		while( ( last = header.find( "\r\n", first ), last ) > first ) {
			std::string line = header.substr( first, last );
			unsigned int division = line.find( ":" );
			
			fields.push_back( std::make_tuple( line.substr( 0, division ), line.substr( division + 2 ) ) );
			

			first = last + 2;
		}
	}
}

std::string RequestHeader::to_string() {
	std::string request( "" );

	request += firstLine + "\r\n";

	// Standart fields
	if( !A_IM.empty() ) {
		request += "A-IM: ";
		request += A_IM + "\r\n";
	}
	if( !Accept.empty() ) {
		request += "Accept: ";
		request += Accept + "\r\n";
	}
	if( !Accept_Charset.empty() ) {
		request += "Accept-Charset: ";
		request += Accept_Charset + "\r\n";
	}
	if( !Accept_Encoding.empty() ) {
		request += "Accept-Encoding: ";
		request += Accept_Encoding + "\r\n";
	}
	if( !Accept_Language.empty() ) {
		request += "Accept-Language: ";
		request += Accept_Language + "\r\n";
	}
	if( !Accept_Datetime.empty() ) {
		request += "Accept-Datetime: ";
		request += Accept_Datetime + "\r\n";
	}
	if( !Accept_Control_Request_Method.empty() ) {
		request += "Accept-Control-Request-Method: ";
		request += Accept_Control_Request_Method + "\r\n";
	}
	if( !Accept_Control_Request_Headers.empty() ) {
		request += "Accept-Control-Request-Headers: ";
		request += Accept_Control_Request_Headers + "\r\n";
	}
	if( !Authorization.empty() ) {
		request += "Authorization: ";
		request += Authorization + "\r\n";
	}
	if( !Cache_Control.empty() ) {
		request += "Cache-Control: ";
		request += Cache_Control + "\r\n";
	}
	if( !Connection.empty() ) {
		request += "Connection: ";
		request += Connection + "\r\n";
	}
	if( !Content_Length.empty() ) {
		request += "Content-Length: ";
		request += Content_Length + "\r\n";
	}
	if( !Content_MD5.empty() ) {
		request += "Content-MD5: ";
		request += Content_MD5 + "\r\n";
	}
	if( !Content_Type.empty() ) {
		request += "Content-Type: ";
		request += Content_Type + "\r\n";
	}
	if( !Cookie.empty() ) {
		request += "Cookie: ";
		request += Cookie + "\r\n";
	}
	if( !Date.empty() ) {
		request += "Date: ";
		request += Date + "\r\n";
	}
	if( !Expect.empty() ) {
		request += "Expect: ";
		request += Expect + "\r\n";
	}
	if( !Forwarded.empty() ) {
		request += "Forwarded: ";
		request += Forwarded + "\r\n";
	}
	if( !From.empty() ) {
		request += "From: ";
		request += From + "\r\n";
	}
	if( !Host.empty() ) {
		request += "Host: ";
		request += Host + "\r\n";
	}
	if( !If_Match.empty() ) {
		request += "If-Match: ";
		request += If_Match + "\r\n";
	}
	if( !If_Modified_Since.empty() ) {
		request += "If-Modified-Since: ";
		request += If_Modified_Since + "\r\n";
	}
	if( !If_None_Match.empty() ) {
		request += "If-None-Match: ";
		request += If_None_Match + "\r\n";
	}
	if( !If_Range.empty() ) {
		request += "If-Range: ";
		request += If_Range + "\r\n";
	}
	if( !If_Unmodified_Since.empty() ) {
		request += "If-Unmodified-Since: ";
		request += If_Unmodified_Since + "\r\n";
	}
	if( !Max_Forwards.empty() ) {
		request += "Max-Forwards: ";
		request += Max_Forwards + "\r\n";
	}
	if( !Origin.empty() ) {
		request += "Origin: ";
		request += Origin + "\r\n";
	}
	if( !Pragma.empty() ) {
		request += "Pragma: ";
		request += Pragma + "\r\n";
	}
	if( !Proxy_Authorization.empty() ) {
		request += "Proxy-Authorization: ";
		request += Proxy_Authorization + "\r\n";
	}
	if( !Range.empty() ) {
		request += "Range: ";
		request += Range + "\r\n";
	}
	if( !Referer.empty() ) {
		request += "Referer: ";
		request += Referer + "\r\n";
	}
	if( !TE.empty() ) {
		request += "TE: ";
		request += TE + "\r\n";
	}
	if( !User_Agent.empty() ) {
		request += "User-Agent: ";
		request += User_Agent + "\r\n";
	}
	if( !Upgrade.empty() ) {
		request += "Upgrade: ";
		request += Upgrade + "\r\n";
	}
	if( !Via.empty() ) {
		request += "Via: ";
		request += Via + "\r\n";
	}
	if( !Warning.empty() ) {
		request += "Warning: ";
		request += Warning + "\r\n";
	}

	// Non-standart but common fields
	if( !Upgrade_Insecure_Requests.empty() ) {
		request += "Upgrade-Insecure-Requests: ";
		request += Upgrade_Insecure_Requests + "\r\n";
	}
	if( !X_Requested_With.empty() ) {
		request += "X-Requested-With: ";
		request += X_Requested_With + "\r\n";
	}
	if( !DNT.empty() ) {
		request += "DNT: ";
		request += DNT + "\r\n";
	}
	if( !X_Forwarded_For.empty() ) {
		request += "X-Forwarded-For: ";
		request += X_Forwarded_For + "\r\n";
	}
	if( !X_Forwarded_Host.empty() ) {
		request += "X-Forwarded-Host: ";
		request += X_Forwarded_Host + "\r\n";
	}
	if( !X_Forwarded_Proto.empty() ) {
		request += "X-Forwarded-Proto: ";
		request += X_Forwarded_Proto + "\r\n";
	}
	if( !Front_End_Https.empty() ) {
		request += "Front-End-Https: ";
		request += Front_End_Https + "\r\n";
	}
	if( !X_HTTP_Method_Override.empty() ) {
		request += "X-HTTP-Method-Override: ";
		request += X_HTTP_Method_Override + "\r\n";
	}
	if( !X_Att_Deviceid.empty() ) {
		request += "X-Att-Deviceid: ";
		request += X_Att_Deviceid + "\r\n";
	}
	if( !x_wap_profile.empty() ) {
		request += "x-wap-profile: ";
		request += x_wap_profile + "\r\n";
	}
	if( !Proxy_Connection.empty() ) {
		request += "Proxy-Connection: ";
		request += Proxy_Connection + "\r\n";
	}
	if( !X_UIDH.empty() ) {
		request += "X-UIDH: ";
		request += X_UIDH + "\r\n";
	}
	if( !X_Csrf_Token.empty() ) {
		request += "X-Csrf-Token: ";
		request += X_Csrf_Token + "\r\n";
	}
	if( !X_Request_ID.empty() ) {
		request += "X-Request-ID: ";
		request += X_Request_ID + "\r\n";
	}
	if( !X_Correlation_ID.empty() ) {
		request += "X-Correlation-ID: ";
		request += X_Correlation_ID + "\r\n";
	}

	request += "\r\n";
	request += body;

	return request;
}

};
