all: ../Graph.o ../heap2k.o generate_instance.o
	g++  ../Graph.o ../heap2k.o generate_instance.o -o generate_instance

../heap.o: ../heap2k.cpp ../heap2k.h
	g++ -c ../heap2k.cpp -o ../heap2k.o

../Graph.o: ../Graph.cpp ../Graph.h 
	g++ -c ../Graph.cpp -o ../Graph.o

generate_instance.o: generate_instance.cpp
	g++ -c generate_instance.cpp -o generate_instance.o

clean:
	rm -f *.o
	rm -f prog
