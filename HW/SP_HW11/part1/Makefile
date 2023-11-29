all: server client

server: usock_server.c lookup5.o
	gcc usock_server.c lookup5.o -o usock_server

client: main.c lookup7.o
	gcc main.c lookup7.o -o usock_client

lookup5.o: lookup5.c
	gcc -c lookup5.c

lookup7.o: lookup7.c
	gcc -c lookup7.c

clean:
	rm -f *.o usock_server usock_client TUNNEL