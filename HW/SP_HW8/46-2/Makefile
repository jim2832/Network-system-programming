all: server client

server: dep
	gcc *.o svmsg_file_server.c -o server

client: dep
	gcc *.o svmsg_file_client.c -o client

dep: error_functions.c get_num.c
	gcc error_functions.c -c
	gcc get_num.c -c

clean:
	rm -f *.o server client