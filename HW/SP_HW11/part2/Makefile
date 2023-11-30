all: server client

server: isock_server.c lookup5.o
	gcc isock_server.c lookup5.o -o isock_server

client: main.c lookup8.o
	gcc main.c lookup8.o -o isock_client

lookup5.o: lookup5.c
	gcc -c lookup5.c

lookup8.o: lookup8.c
	gcc -c lookup8.c

clean:
	rm -f *.o isock_server isock_client