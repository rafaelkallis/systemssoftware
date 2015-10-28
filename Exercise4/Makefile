all: med_filt

med_filt: main.o image_matrix.o timetracker.o
	g++ -fopenmp main.o image_matrix.o timetracker.o -o med_filt

main.o: main.cpp image_matrix.hpp
	g++ -fopenmp -c main.cpp

image_matrix.o: image_matrix.cpp image_matrix.hpp
	g++ -c image_matrix.cpp

timetracker.o: timetracker.cpp timetracker.h
	g++ -c timetracker.cpp

clean:
	rm -rf *.o med_filt
