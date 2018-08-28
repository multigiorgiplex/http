#include <stdio.h>
//#include <multigiorgiplex/http.h>
#include "../http.h"

void main()
{
    char asd[100];
    char head[100];
    char host[] = "www.theansweris42.com";

	http_gen_GETheader (host, head);
    http_gen_GETrequest("un.pdf", asd, head);

    printf ("|%s|", asd);
}
