all: dep
	gcc *.o sig_speed_sigsuspend.c -o prog

dep: error_functions.c get_num.c
	gcc error_functions.c -c
	gcc get_num.c -c

clean:
	rm -f *.o prog