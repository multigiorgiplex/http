http: http.c http.h
	gcc -Wall -Werror -fpic -c http.c
	gcc -shared -o libhttp.so http.o
	
clean:
	rm http.o libhttp.so

