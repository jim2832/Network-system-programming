all: dep
	gcc *.o main.c -o main -lm

dep: producer.c consumer.c
	gcc producer.c -c
	gcc consumer.c -c

clean:
	rm -rf main *.o