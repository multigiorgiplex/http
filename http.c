#include "http.h"

void http_initialize (int debug)
{
	if (debug >= 0 && debug <= 2)
	{
		_http_debug = debug;
		_http_flag_init = 1;

		printf ("[" __FILE__ "] i! libhttp.so inizializzata (debug level: %d)\n", debug);
	}
	else
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! Livello debug non in range\n", __LINE__);
	}
}

//int http_parse_request (char *request_buffer, char *request_type, char *requested_file, char *request_arguments)
int http_parse_request (char *request_buffer, struct http_request *request)
{
	char line[100];
	
	char *split_line, *split_line_prev;
	char *split_column;

	short is_post = 0;

	if (!_http_flag_init)
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! Libreria non inizializzata\n", __LINE__);
		return -1;
	}
	
	//Azzero i buffer
	memset (request, 0, sizeof (struct http_request));

	split_line = strchr (request_buffer, '\r');
	if (split_line == NULL)
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! Request invalida! [E-1]\n", __LINE__);
		return -2;
	}
	*((char *) mempcpy (line, request_buffer, split_line-request_buffer)) = 0;
	
	printf ("len: %ld riga: %s\n",split_line-request_buffer, line);
	
	if (_http_debug >= 2) printf ("[" __FILE__ "] i! HTTP Request:\n%s", request_buffer);

	// 		"GET /file.asd HTTP/1.1"
	//		 →	↑	
	split_column = memccpy (request->method, line, ' ', 5);
	if (split_column == NULL)
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! Request invalida! [E-2]\n", __LINE__);
		return -2;
	}
	*(split_column -1) = 0;	//Tolgo lo spazio
	split_column = line + (split_column - request->method);
	
	// 		"GET /file.asd HTTP/1.1"
	//			 →		  ↑	
	split_column = memccpy (request->filename, split_column, ' ', HTTP_REQUEST_FILE_LEN);
	if (split_column == NULL)
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! Request invalida! [E-3]\n", __LINE__);
		return -2;
	}

	//Controllo argomenti per metodo
	
	if (!memcmp (request->method, "GET\0", 4))	// Se e' una richiesta GET
	{
		//Controllo se ci sono argomenti
		split_column = strchr (request->filename, '?');
		if (split_column != NULL)	//Ci sono argomenti GET
		{
			memccpy (request->arguments, split_column+1, ' ', HTTP_ARGUMENTS_LEN);
			*split_column = 0;
		}
	}
	else if (!memcmp (request->method, "POST\0", 5))	// Se e' una richiesta POST
	{
		is_post = 1;
	}
	else
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! Method non supportato!\n", __LINE__);				//response 405!!!!!
		return -3;
	}

	//Analizzo le altre righe della request
	do
	{
		split_line_prev = split_line+2;	//+2 because \r\n

		if (*split_line_prev == '\r' && *(split_line_prev+1) == '\n')
		{
			if (*(split_line_prev+2) != 0)	//There is a message body
			{
				if (is_post)
				{
					if (request->headers.content_lenght > 0)
						memccpy (request->arguments, split_line_prev+2, '\r', request->headers.content_lenght);
					else					
						memccpy (request->arguments, split_line_prev+2, '\r', HTTP_ARGUMENTS_LEN);
					return 0;
				}
				//else ignore the message body	RFC2616 - 4.3
			}
			else 	//Request ended
			{
				return 0;
			}
		}
		
		split_line = strchr (split_line_prev, '\r');
		if (split_line != NULL)
		{
			*((char *) mempcpy (line, split_line_prev, split_line - split_line_prev)) = 0;

			if (!memcmp (line, "Host: ", 6))
				memccpy (request->headers.host, strchr (line, ' ')+1, '\r', HTTP_HEADER_FIELD_LEN);	//Il terminatore gia cel'ha perche la stringa e stata memsettata a 0 prima

			if (!memcmp (line, "Content-Lenght: ", 16))
				request->headers.content_lenght = (unsigned int) strtol (strchr (line, ' ')+1, NULL, 10);
			

		}
		else //Ultima riga - non dovrebbe mai succedere, una request se non ha message-body deve terminare con CRLF, e quindi viene individuato prima
		{
			fprintf (stderr, "[" __FILE__ ":%u] E! Request invalida! [E-4]\n", __LINE__);
			return -2;
		}
		
		
		
	} while (split_line != NULL);

	return 0;
}

int http_get_file (char *path, char *b)
{
	struct stat s;
	FILE *f;
	size_t r;
	char p[HTTP_REQUEST_FILE_LEN];	

	if (!_http_flag_init)
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! Libreria non inizializzata\n", __LINE__);
		return -1;
	}

	//Azzero i buffer
	memset (p, 0, HTTP_REQUEST_FILE_LEN);

	if (stat (path, &s))	//man 2 stat
	{
		if (errno == ENOENT)
		{
			if (path[0] == 0)
			{
				printf ("mi richiamo con il file scritto\n");
				return http_get_file ("index.html", b);
			}
			
			fprintf (stderr, "[" __FILE__ ":%u] E! File %s non trovato\n", __LINE__, path);
			return 404;
		}
		
		fprintf (stderr, "[" __FILE__ ":%u] E! %s\n", __LINE__, strerror(errno));
		return -1;
	}

	//man 7 inode
	if ((s.st_mode & 0170000) == S_IFREG)	// è un file regolare
	{
		;
	}
	else if ((s.st_mode & 0170000) == S_IFDIR)	// è una directory
	{
		if (_http_debug) printf ("[" __FILE__ ":%u] i! Il file %s è una cartella\n", __LINE__, path);
		printf ("mi richiamo aggiungendo il file\n");

		strcpy (p, path);
		
		if (p[strlen(p)-1] != '/') p[strlen(p)] = '/';
		strcat (p, "index.html");
		
		if (http_get_file (p, b) == 200)
			return 200;
		else
			return 0;
	}
	else
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! Il file %s non è né un file regolare né una directory\n", __LINE__, path);
		return -1;
	}

	// il file è un file regolare
	f = fopen (path, "r");
	if (f == NULL)
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! %s\n", __LINE__, strerror(errno));
		return -1;
	}

	r = fread (b, 1, HTTP_FILE_MAX_LEN, f);
	fclose (f);
	b[r] = 0;

	if (_http_debug) printf ("[" __FILE__ ":%u] i! Letto file %s correttamente\n", __LINE__, path);
	if (_http_debug >= 2) _http_buffer_dump (b, r);

	return 200;
}


void http_gen_GETrequest (char *file_requested, char *output_buffer, char *headers_buffer)
{
	size_t str_lenght;
	char uri[HTTP_REQUEST_FILE_LEN];

	uri[0] = '/';
	if (file_requested == NULL)
		uri[1] = 0;
	else
		strcpy (uri+1, file_requested);
		//Terminatore?

	// Secondo RFC2616 5.1
	//Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
	sprintf (output_buffer, "GET %s HTTP/1.1\r\n", uri);
	//Terminatore?

	if (headers_buffer == NULL)
		return;

	strcat (output_buffer, headers_buffer);
	str_lenght = strlen (output_buffer);
	*(output_buffer+str_lenght  ) = '\r';
	*(output_buffer+str_lenght+1) = '\n';
	*(output_buffer+str_lenght+2) = '\0';
}


void http_gen_GETheader (char *host, char *output_buffer)
{
	sprintf (output_buffer, "Host: %s\r\n", host);
}


void http_gen_GETresponse (char *output_buffer, char *server_name, unsigned response_code, char connection_closed, unsigned content_lenght, char *content_buffer)
{
	size_t len = 0;
		
	switch (response_code)
	{
		case 200:
			len += sprintf (output_buffer+len, HTTP_RESPONSE_200);
			break;
		case 404:
			len += sprintf (output_buffer+len, HTTP_RESPONSE_404);
			break;
	}
	
	len += sprintf (output_buffer+len, "Server: %s\r\n", server_name);
	
	if (connection_closed)
		len += sprintf (output_buffer+len, "Connection: Closed\r\n");
	
	len += sprintf (output_buffer+len, "Content-Lenght: %u\r\n", content_lenght);
	
	if (content_lenght > 0 && content_buffer != NULL)
		len += sprintf (output_buffer+len, "\r\n%s", content_buffer);
	
	sprintf (output_buffer+len, "\r\n");
}







void _http_buffer_dump (char *b, ssize_t l)
{
	ssize_t i;

	for (i=0; i<l; i++)
	{
		printf ("%02x ", *(b+i));
	}
	printf ("\b\n");
}
