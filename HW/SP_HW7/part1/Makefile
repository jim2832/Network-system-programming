all: dep
	gcc *.o sig_speed_sigsuspend.c -o origin

new: dep
	gcc *.o sig_speed_sigsuspend.c -DNEW -o new_version

dep: error_functions.c get_num.c new_version.c
	gcc error_functions.c -c
	gcc get_num.c -c
	gcc new_version.c -c

clean:
	rm -f *.o origin new_version *.txt