all: simple-test 

simple-test: simple-test.o byos.o
	gcc -Wall $^ -o $@

%.o: %.c byos.h
	gcc -Wall -c $<

clear:
	rm *.o simple-test