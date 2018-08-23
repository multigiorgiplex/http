install_lib_dir = /usr/local/lib/
install_header_dir = /usr/local/include/multigiorgiplex/
lib_soname = libhttp.so

http: http.c http.h
	gcc -Wall -Werror -fpic -c http.c
	gcc -shared -o $(lib_soname) http.o

install:
	cp $(lib_soname) $(install_lib_dir)
	if [ ! -e $(install_header_dir) ]; then mkdir $(install_header_dir); fi
	cp http.h $(install_header_dir)

uninstall:
	rm $(install_lib_dir)$(lib_soname)
	rm $(install_header_dir)http.h
	if [ ! `ls -A $(install_header_dir)` ]; then rmdir $(install_header_dir); fi  
	
clean:
	rm http.o $(lib_soname)

