#ifndef _HTTP_H
	#define _HTTP_H

#define _GNU_SOURCE		//for mempcpy()

#ifdef __i386__
	#define _PRINTF_SSIZE_T	"%d"
#endif
#ifdef __amd64__
	#define _PRINTF_SSIZE_T	"%lu"
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>  	//funzione per la gestione degli errori
	#include <sys/types.h>	//stat
	#include <sys/stat.h>
	#include <unistd.h>

#define HTTP_REQUEST_FILE_LEN	100		// Lunghezza massima file richiesto HTTP
#define	HTTP_FILE_MAX_LEN 		65535	// Lunghezza massima del file da inviare
#define HTTP_HEADER_FIELD_LEN	100		// Lunghezza massima dei FILED degli header
#define HTTP_ARGUMENTS_LEN		100		// Lunghezza massima del buffer degli argomenti

#define HTTP_VERSION		"HTTP/1.1"

//	RFC2612 - 6.1.1
#define HTTP_REASON_PHRASE_200	"OK"
#define HTTP_REASON_PHRASE_404	"Not Found"

#define HTTP_RESPONSE_200	"HTTP/1.1 200 OK\r\n"	//deprecato
#define HTTP_RESPONSE_404	"HTTP/1.1 404 Not Found\r\n"	//deprecato

int _http_flag_init = 0;
int _http_debug;

struct http_headers
{
	//	general-header	(RFC2616 - 4.5)
	short unsigned connection;	//0 = Keep alive, 1 = Closed
	
	//	request-header	(RFC2616 - 5.3)
	char host[HTTP_HEADER_FIELD_LEN];

	//	response-header	(RFC2616 - 6.2)
	char server[HTTP_HEADER_FIELD_LEN];
	char www_authenticate[HTTP_HEADER_FIELD_LEN];
	
	//	entity-header	(RFC2616 - 7.1)
	unsigned content_lenght;	
};

struct http_request
{
	char method[8];
	char filename[HTTP_ARGUMENTS_LEN];
	struct http_headers headers;	
	char arguments[HTTP_ARGUMENTS_LEN];
};

struct http_response
{
	//char method[8];	non dovrebbe servire
	unsigned status_code;
	struct http_headers headers;
	char *message_body;
};

/*	Inizializza la libreria
 *	Argomenti:
 * 		debug:	Imposta il livello di debugging
 * 				0: Spento
 * 				1: Visualizza messaggi
 * 				2: Visualizza buffer
*/
void http_initialize(int debug);

/*	Analizza una request HTTP compresa di header partendo da un buffer
 *	Argomenti:
 * 		*request_buffer:	Buffer in cui e' locata la request
 * 		*request:			Struttura dati in cui inserire i dati della request
 * 	Ritorno:
 * 		0:		request correttamente analizzata
 * 		-1:		Libreria non inizializzata
 * 		-2:		Request invalida
 * 		-3:		Metodo non supportato
*/
int http_parse_request (char *request_buffer, struct http_request *request);

int http_generate_response (char *response_buffer, struct http_response response);

/*	Cerca e legge il file richiesto
 * 	Argomenti:
 * 		path:	Directory del file
 * 		b:		Buffer di lettura del file
 * 	Ritorno:
 * 		200:	File regolare, aperto e letto
 * 		404:	File non trovato
 * 		0:		Path è una directory
 * 		-1:		Errore
*/
int http_get_file (char *path, char *b);

/*	Genera una richesta GET HTTP
 * 	Argomenti:
 * 		file_requested:		File da richiedere (se NULL viene richiesto /)
 * 		output_buffer:		Buffer su cui scrivere la richiesta
 * 		headers_buffer:		Headers generati da http_gen_GETheader() da copiare nella richiesta (può essere NULL)
*/
// servirà Host:?
void http_gen_GETrequest (char *file_requested, char *output_buffer, char *headers_buffer);

/*	Genera un header valido per http_gen_GETrequest()
 * 	Argomenti:
 * 		host:				L'hostname da riportare nella richiesta
 * 		output_buffer:		Buffer su cui scrivere gli header
*/
void http_gen_GETheader (char *host, char *output_buffer);


void http_gen_GETresponse (char *output_buffer, char *server_name, unsigned response_code, char connection_closed, unsigned content_lenght, char *content_buffer);


void _http_buffer_dump (char *b, ssize_t l);

#endif
