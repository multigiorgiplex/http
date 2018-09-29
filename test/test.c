#include <stdio.h>
#include <string.h>
//#include <multigiorgiplex/http.h>
#include "../http.h"

void main()
{
	struct http_request request;
	struct http_response response;
	//char request_buffer[] = "GET /index.html?method=get HTTP/1.1\r\nHost: theansweris42.com\r\nConnection: Closed\r\n\r\n";
	char request_buffer_pre[] = "POST /index.html HTTP/1.1\r\nHost: theansweris42.com\r\nConnection: Closed\r\nContent-Lenght: 11\r\n\r\nmethod=post\r\n";
	char request_buffer[1024];
	char request_msgbody[1024];
	char response_buffer[1024];
	

	http_initialize (2);

	/*ttp_parse_request (request_buffer, &request);

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
	*/
	
    response.status_code = 200;
    strcpy (response.headers.host, request.headers.host);
    response.headers.connection = 0;
    response.message_body = request_buffer_pre;
	http_generate_response (response_buffer, response);
	printf ("Response generata:\n%s\n\n", response_buffer);

	strcpy (request.method, "POST");
	strcpy (request.uri, "/index.html");
	strcpy (request.headers.host, "42istheanswer.com");
	request.headers.connection = 1;
	strcpy (request_msgbody, "alpha=A&beta=B");
	request.message_body = request_msgbody;
	http_generate_request (request_buffer, request);
	printf ("Request generata:\n%s\n\n", request_buffer);
    
}
