all: generate_instance.o 
	g++  ../Graph.o ../heap2k.o generate_instance.o -o generate_instance



generate_instance.o: generate_instance.cpp 
	g++ -c generate_instance.cpp -o generate_instance.o

clean:
	rm -f *.o
	rm -f prog
