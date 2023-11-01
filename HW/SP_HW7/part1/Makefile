all: dep
	gcc *.o sig_speed_sigsuspend.c -o origin

new: dep
	gcc *.o new.c -DNEW -o new

dep: new.c error_functions.c get_num.c
	gcc new.c -c
	gcc error_functions.c -c
	gcc get_num.c -c

clean:
	rm -f *.o origin new