all: server client

server: dep
	gcc *.o fifo_seqnum_server.c -o server

client: dep
	gcc *.o fifo_seqnum_client.c -o client
	gcc *.o misbehaving_client.c -o misbehaving_client

dep: error_functions.c get_num.c
	gcc error_functions.c -c
	gcc get_num.c -c

clean:
	rm -f *.o server client misbehaving_client