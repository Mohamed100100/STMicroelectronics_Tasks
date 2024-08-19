
heap_test: heap.o stress_test.o 
	@gcc heap.o stress_test.o -o heap_test

heap.o: heap.c heap.h
	@gcc -o heap.o -c heap.c 

stress_test.o: heap.h stress_test.c
	@gcc -o stress_test.o -c stress_test.c


clean: 
	@rm heap.o 
	@rm stress_test.o
	@rm heap_test


.PHONY:clean
