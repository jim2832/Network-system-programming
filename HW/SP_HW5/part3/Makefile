all:
	gcc convert.c -o convert
	./convert dict myfixrec
	gcc main.c lookup1.c -o file_lookup
clean:
	rm -f convert myfixrec file_lookup