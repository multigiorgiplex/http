#include <stdio.h>
#include <multigiorgiplex/http.h>

void main()
{
    char asd[59];

    http_gen_GETrequest("", asd);

    printf ("%s\n", asd);
}
