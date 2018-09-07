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

    printf ("|%s|\n", asd);
    
    http_gen_GETresponse (asd, "test", 200, 1, 42, "the answer is always 42!");
    printf ("|%s|\n", asd);
    
}
