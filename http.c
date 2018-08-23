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

int http_parse_request (char *r, char *t, char *f, char *a)
{
	char *split;
	char *split2;

	char *cpy;
	char *str;

	if (!_http_flag_init)
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! Libreria non inizializzata\n", __LINE__);
		return -1;
	}
	
	//Azzero i buffer
	memset (t, 0, 5);
	memset (f, 0, HTTP_REQUEST_FILE_LEN);
	memset (a, 0, 100);	
	
	if (_http_debug >= 2) printf ("[" __FILE__ "] i! HTTP Header:\n%s\n", r);

	split = strchr (r, '\r');
	if (split == NULL)
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! Request invalida! [E-1]\n", __LINE__);
		return -1;
	}
	*split = 0;
	
	if (_http_debug) printf ("[" __FILE__ "] i! HTTP Request: %s\n", r);	// r = "GET /file.asd HTTP/1.1"

		// Trova il primo spazio e lo sostituisce con un fine stringa
		// 		"GET /file.asd HTTP/1.1"
		//			↑	
		split2 = strchr (r, ' ');
		if (split2 == NULL)
		{
			fprintf (stderr, "[" __FILE__ ":%u] E! Request invalida! [E-2]\n", __LINE__);
			return -2;
		}
		*split2 = 0;

		// Copia la stringa in t fino al fine stringa
		cpy = memccpy (t, r, 0, 5);
		if (cpy == NULL)
		{
			fprintf (stderr, "[" __FILE__ ":%u] E! Request invalida! [E-3]\n", __LINE__);
			return -3;
		}

		// Copia in f dal carattere dopo il fine stringa fino allo spazio
		// 		"GET /file.asd HTTP/1.1"
		//			 →		  ↑	
		cpy = memccpy (f, split2+1, ' ', HTTP_REQUEST_FILE_LEN);
		if (cpy == NULL)
		{
			fprintf (stderr, "[" __FILE__ ":%u] E! Request invalida! [E-4]\n", __LINE__);
			return -4;
		}
		*(cpy-1) = 0;
	
	//Controllo se ci sono argomenti di tipo GET
	split2 = strchr (f, '?');
	if (split2 != NULL)	//Ci sono argomenti GET
	{
		memccpy (a, split2+1, 0, 100);
		memset (split2, 0, strlen (split2));	//pulisco il buffer file
	}

	if (!memcmp (t, "GET\0", 5))	// Se e' una richiesta GET
	{
		;
	}
	else if (!memcmp (t, "POST\0", 5))	// Se e' una richiesta GET
	{
		//	ANALIZZO IL RESTO DELL'HEADER REQUEST
		while (split != NULL)
		{
			str = split+2;
			split = strchr (str, '\r');
			//if (split != NULL) *split = 0;
		}
		memccpy (a, str, 0, 100);
	}
	else
	{
		fprintf (stderr, "[" __FILE__ ":%u] E! Request non supportato [E-5]\n", __LINE__);				//response 405!!!!!
		return -5;
	}

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


void http_gen_GETrequest (char *f, char *b)
{
	//int l;
	char uri[HTTP_REQUEST_FILE_LEN];

	uri[0] = '/';
	if (f == NULL)
		uri[1] = 0;
	else
		strcpy (uri+1, f);
		//Terminatore?

	// Secondo RFC2616 5.1
	//Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
	sprintf (b, "GET %s HTTP/1.1\r\n", uri);
	//Terminatore?
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
