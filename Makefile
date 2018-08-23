install_lib_dir = /usr/lib/multigiorgiplex/
install_header_dir = /usr/local/include/multigiorgiplex/
ld_conf_file = /etc/ld.so.conf.d/multigiorgiplex.conf
lib_soname = libhttp.so

http: http.c http.h
	gcc -Wall -Werror -fpic -c http.c
	gcc -shared -o $(lib_soname) http.o

install:
	if [ ! -e $(install_lib_dir) ]; then \
		mkdir $(install_lib_dir); \
		echo "$(install_lib_dir)" > $(ld_conf_file); \
	fi
	if [ ! -e $(install_header_dir) ]; then \
		mkdir $(install_header_dir); \
	fi
	cp $(lib_soname) $(install_lib_dir)	
	cp http.h $(install_header_dir)
	ldconfig

uninstall:
	rm $(install_lib_dir)$(lib_soname)
	rm $(install_header_dir)http.h
	if [ ! `ls -A $(install_lib_dir)` ]; then \
		rmdir $(install_lib_dir); \
		rm $(ld_conf_file); \
	fi  
	if [ ! `ls -A $(install_header_dir)` ]; then \
		rmdir $(install_header_dir); \
	fi
	ldconfig
	
clean:
	rm http.o $(lib_soname)

