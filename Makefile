CXX=g++
CXXFLAGS=-O2 -Wall

all: r2-converter

r2-converter: core util
	$(CXX) R2Song.o R2Steps.o SMWriter.o StepsHelper.o Utilities.o Main.o -o r2-converter

core:
	$(CXX) -c $(CXXFLAGS) Main.cpp R2Song.cpp R2Steps.cpp

util:
	$(CXX) -c $(CXXFLAGS) SMWriter.cpp StepsHelper.cpp Utilities.cpp

clean:
	rm *.o r2-converter
