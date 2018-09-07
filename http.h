#ifndef _HTTP_H
	#define _HTTP_H

#include <stdio.h>
#include <string.h>
#include <errno.h>  	//funzione per la gestione degli errori
	#include <sys/types.h>	//stat
	#include <sys/stat.h>
	#include <unistd.h>

#define HTTP_REQUEST_FILE_LEN	100		// Lunghezza massima file richiesto HTTP
#define	HTTP_FILE_MAX_LEN 		65535	// Lunghezza massima del file da inviare

#define HTTP_RESPONSE_200	"HTTP/1.1 200 OK\r\n"
#define HTTP_RESPONSE_404	"HTTP/1.1 404 Not Found\r\n"

int _http_flag_init = 0;
int _http_debug;

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
 * 		*request_buffer:		Buffer in cui e' locata la request
 * 		*request_type:			Buffer su cui salvare il tipo di request "GET", "POST" ...
 * 		*requested_file:		Buffer su cui viene salvato il file richiesto dalla request "/index.html"
 * 		*request_argumetns:		Buffer su cui vengono salvati gli argomenti GET o POST
 * 	Ritorno:
 * 		0:		request correttamente analizzata
 * 		-1:		Request invalida: la richiesta non contiene ritorni a capo
 * 		-2:		Request invalida: la prima riga "GET / HTTP/1.1" non contiene spazi
 * 		-3:		Request invalida: metodo HTTP lungo piu' di 5 caratteri
 * 		-4:		Request invalida: filename richiesto supera HTTP_REQUEST_FILE_LEN caratteri
 *		-5:		Metodo request non supportato
*/
int http_parse_request (char *request_buffer, char *request_type, char *requested_file, char *request_arguments);

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
