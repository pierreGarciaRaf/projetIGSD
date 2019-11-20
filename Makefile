# Le compilateur à utiliser
CXX = g++
# Les options du compilateur
CXXFLAGS = -Wall -std=c++11 -g -I/u/usr/local/include/ -lglfw -lGLEW -framework OpenGL



quickAndDirtyIteration: quickAndDirtyIteration.cpp fileReader.o

fileReader.o: fileReader.cpp fileReader.hpp