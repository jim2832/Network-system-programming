all: dep
	gcc *.o svshm_xfr_writer.c -o svshm_xfr_writer
	gcc *.o svshm_xfr_reader.c -o svshm_xfr_reader

dep: error_functions.c get_num.c binary_sems.c
	gcc error_functions.c -c
	gcc get_num.c -c
	gcc binary_sems.c -c

clean:
	rm -rf svshm_xfr_writer svshm_xfr_reader *.o