Assignment2: a1.o
	g++ Assign-2.o -o Assignment-2
Assign-2.o: Assign-2.cpp
	g++ Assign-2.cpp -c 
clean:
	rm *.o Assignment-2
