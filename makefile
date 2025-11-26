# Makefile for mygllib
# Y. Liow
#------------------------------------------------------------------------------
# Macros
#------------------------------------------------------------------------------
CXX       = g++
CXXFLAGS  = -g -Wall
LINK      = g++
LINKFLAGS = -lGL -lGLU -lglut 
OBJS      =

main.exe: *.cpp *.h
	$(CXX) *.cpp $(CXXFLAGS) $(LINKFLAGS) -o main.exe
#------------------------------------------------------------------------------
# Object files
#------------------------------------------------------------------------------
#config.o: config.h config.cpp  
#	$(CXX) $(CXXFLAGS) config.cpp -c -o config.o
#
#View.o: View.h View.cpp
#	$(CXX) $(CXXFLAGS) View.cpp -c -o View.o
#
#SingletonView.o: SingletonView.h SingletonView.cpp
#	$(CXX) $(CXXFLAGS) SingletonView.cpp -c -o SingletonView.o
#
#Reshape.o: Reshape.h Reshape.cpp
#	$(CXX) $(CXXFLAGS) Reshape.cpp -c -o Reshape.o
#
#Keyboard.o: Keyboard.h Keyboard.cpp
#	$(CXX) $(CXXFLAGS) Keyboard.cpp -c -o Keyboard.o

#------------------------------------------------------------------------------
# Utilities
#------------------------------------------------------------------------------
r:
	./main.exe
clean:
	rm -f main.exe
c:
	rm -f main.exe
