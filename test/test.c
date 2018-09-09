#include <stdio.h>
#include <string.h>
//#include <multigiorgiplex/http.h>
#include "../http.h"

void main()
{
	struct http_request request;
	//char request_buffer[] = "GET /index.html?method=get HTTP/1.1\r\nHost: theansweris42.com\r\nConnection: Closed\r\n\r\n";
	char request_buffer[] = "POST /index.html HTTP/1.1\r\nHost: theansweris42.com\r\nConnection: Closed\r\nContent-Lenght: 11\r\n\r\nmethod=post\r\n";

	http_initialize (2);

	http_parse_request (request_buffer, &request);

	printf ("\
		Method:		%s\n\
		File:		%s\n\
		Host:		%s\n\
		Content-Lenght:	%u\n\
		Arguments:	%s\n",

		request.method,
		request.filename,
		request.headers.host,
		request.headers.content_lenght,
		request.arguments
	);
	
    
}
