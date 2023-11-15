all: dep
	gcc *.o mmcat.c -o mmcat

dep: error_functions.c get_num.c
	gcc error_functions.c -c
	gcc get_num.c -c

clean:
	rm -rf *.o mmcat