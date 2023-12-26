all: systemv.o
	gcc systemv.o main.c ${FLAGS} -o target

systemv.o: systemv.c
	gcc systemv.c ${FLAGS} -c

clean:
	rm target *.o