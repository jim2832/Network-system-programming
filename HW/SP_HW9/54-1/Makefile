all: dep
	gcc *.o posix_shm_xfr_writer.c -o posix_shm_xfr_writer
	gcc *.o posix_shm_xfr_reader.c -o posix_shm_xfr_reader

dep: error_functions.c get_num.c binary_sems.c
	gcc error_functions.c -c
	gcc get_num.c -c
	gcc binary_sems.c -c

clean:
	rm -rf posix_shm_xfr_writer posix_shm_xfr_reader *.o