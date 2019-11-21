# Le compilateur à utiliser
CXX = g++
# Les options du compilateur
CXXFLAGS = -mmacosx-version-min=10.12 -Wall -std=c++11 -g -I/u/usr/local/include/ -lglfw -lGLEW -framework OpenGL



quickAndDirtyIteration: quickAndDirtyIteration.cpp fileReader.o curve.o

curve.o: curve.cpp fileReader.hpp curve.hpp
fileReader.o: fileReader.cpp fileReader.hpp

clean: rm *.o