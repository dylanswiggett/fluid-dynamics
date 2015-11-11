CXX = g++
LINKERS = -lSDL2 -lGLEW -lGL
FLAGS = -Wall -g -std=c++11 -rdynamic -O4

main: main.o Graphics.o Dynamics.o DummyRender.o shader_loader.o
	$(CXX) $(FLAGS) -o main main.o Graphics.o Dynamics.o DummyRender.o shader_loader.o $(LINKERS)

main.o: main.cpp Graphics.hpp Dynamics.hpp
	$(CXX) $(FLAGS) -c -o main.o main.cpp

Graphics.o: Graphics.cpp Graphics.hpp Dynamics.hpp DummyRender.hpp shader_loader.hpp
	$(CXX) $(FLAGS) -c -o Graphics.o Graphics.cpp

Dynamics.o: Dynamics.cpp Dynamics.hpp DummyRender.hpp shader_loader.hpp
	$(CXX) $(FLAGS) -c -o Dynamics.o Dynamics.cpp

DummyRender.o: DummyRender.cpp DummyRender.hpp
	$(CXX) $(FLAGS) -c -o DummyRender.o DummyRender.cpp

shader_loader.o: shader_loader.cpp shader_loader.hpp
	$(CXX) $(FLAGS) -c -o shader_loader.o shader_loader.cpp

clean:
	rm -rf *.o
