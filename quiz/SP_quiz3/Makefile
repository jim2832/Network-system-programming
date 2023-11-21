all: dep
	gcc *.o quiz3.c -o quiz3

dep: error_functions.c get_num.c binary_sems.c
	gcc error_functions.c -c
	gcc get_num.c -c
	gcc binary_sems.c -c

clean:
	rm -rf quiz3 *.o
