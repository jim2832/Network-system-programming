all: server client

server: udp_server.c lookup5.o
	gcc udp_server.c lookup5.o -o udp_server

client: main.c lookup9.o
	gcc main.c lookup9.o -o udp_client

lookup5.o: lookup5.c
	gcc -c lookup5.c

lookup9.o: lookup9.c
	gcc -c lookup9.c

clean:
	rm -f *.o udp_server udp_client